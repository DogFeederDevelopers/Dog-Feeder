/*
 Authors:Tamir Zitman, Almog Shtaigman Thanks to:	giltal
*/

#define BLYNK_TEMPLATE_ID           "TMPLvlo1BP8g"
#define BLYNK_DEVICE_NAME           "DogFeederTemplate"
#define BLYNK_AUTH_TOKEN            "nSgd5nnVgfpmcTq2zCp5MT3FxwnaLfaH"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "time.h"
#include <Servo.h>
#include <stdio.h>
#include <Wire.h>
#include "SPIFFS.h" 

#include <ESPmDNS.h>
#include <WebServer.h>

//Blynk setup
//char ssid[] = "Snir";
//char pass[] = "049832974";

char auth[] = BLYNK_AUTH_TOKEN;

//Call function to handle wifi setup through html pages
WebServer server(80);
void handle_OnConnect();
void wifiSetup_page();
void toggleIO_page();
void handle_NotFound();

// Time Variables:
int brkTimeH = 7;
int brkTimeM = 30;

int dnrTimeH = 17;
int dnrTimeM = 31;

// Wifi flags
bool wifiSetupPageVisited = false;
bool toggleIOpageVisited = false;
bool spiffsOK = false;
bool blynkHasConfigured = false;

bool schedIsActive;
bool pendingBrk;
bool pendingDnr;
bool tankEmptyNotified;

// NTP server: 
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200;
const int   daylightOffset_sec = 3600;

// Feed Rounds:
int pos = 0;  // variable to store the servo position
int rnds = 5; // The amount of moves for 1 dose
int servedMeals = 0; // The amount of served meals
const int fullTankServs = 4; // The amount of rounds for full container
int mealsLeft = fullTankServs - servedMeals;

// Buttons:
int isDoseBtPressed = 0;     // Flag for manually pressing for  dose
int isResetBtPressed = 0;    // Flag for pressing to reset
const int buttunForFeed = 23;      // Button for manully feed action
const int buttunForResetTank = 22; // Button for resetting Tank + long press will turn on/off auto-Scheduale

// LEDs:
const int redLed = 12;    // Red light - No Food
const int yellowLed = 14; // Yellow light - auto-schedule is off
const int greenLed = 27;  // Green light - System init blinking, feeding

// Servo:
Servo servo;
const int servoAttachPin = 32;

bool BLYNK_ON_CORE_0 = true;


#pragma region Functions
/* --------------- */
// Functions:

void TestBlynk(char auth[], char ssid[], char pass[]) {
    Blynk.begin(auth, ssid, pass);
    while (Blynk.connected() == false) {
    }
    //this is where we start the Blynk.run() loop pinned to core 0, given priority "1" (which gives it thread priority over "0")
    xTaskCreatePinnedToCore(
        blynkLoop,      /* Function to implement the task */
        "blynk core 0", /* Name of the task */
        100000,         /* Stack size in words */
        NULL,           /* Task input parameter */
        1,              /* Priority of the task */
        NULL,           /* Task handle. */
        0);             /* Core where the task should run */

    //Servo setup
    servo.attach(servoAttachPin);

    //LEDs and buttuns
    pinMode(buttunForFeed, INPUT);
    pinMode(buttunForResetTank, INPUT);
    pinMode(redLed, OUTPUT);
    pinMode(yellowLed, OUTPUT);
    pinMode(greenLed, OUTPUT);

    Serial.println("");

    setNextMeal();
}

void printInitTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        writeLog((char*)"critical", (char*)"Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

int getSec() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        writeLog((char*)"critical", (char*)"Failed to obtain time");
        return 0;
    }
    return (int)timeinfo.tm_sec;
}

int getMin() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        writeLog((char*)"critical", (char*)"Failed to obtain time");
        return 0;
    }
    return (int)timeinfo.tm_min;
}

int getHur() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        writeLog((char*)"critical", (char*)"Failed to obtain time");
        return 0;
    }
    return (int)timeinfo.tm_hour;
}

void setNextMeal() // sets the pending breakfast and dinner according to the current time
{
    char timeNowStr[50];
    if (getMin() < 10) {
        sprintf(timeNowStr, "The time right now is: %d:0%d", getHur(), getMin());
    }
    else {
        sprintf(timeNowStr, "The time right now is: %d:%d", getHur(), getMin());
    }
    writeLog((char*)"info", (char*)timeNowStr);

    if (((getHur() > brkTimeH) && (getHur() < dnrTimeH)) || ((getHur() == brkTimeH) && (getMin() > brkTimeM)) || ((getHur() == dnrTimeH) && (getMin() < dnrTimeM))) {
        writeLog((char*)"info", (char*)"Next meal is set to Dinner");
        pendingBrk = false;
        pendingDnr = true;
    }
    else {
        writeLog((char*)"info", (char*)"Next meal is set to Breakfast");
        pendingBrk = true;
        pendingDnr = false;
    }
}

void ChangeSchedMode(int state)
{
    // state 0 - OFF
    if (state == 0)
    {
        Serial.println("Turns off sched and consistently turn on yellow light");
        schedIsActive = false;
        digitalWrite(yellowLed, 1);
    }
    if (state == 1)
    {
        Serial.println("Turns back on sched and turn off yellow light");
        schedIsActive = true;
        setNextMeal();
        digitalWrite(yellowLed, 0);
    }
}

void resetTank()
{
    writeLog((char*)"info", (char*)"Tank was reseted");

    digitalWrite(redLed, LOW);

    //Set the VP to 0
    Blynk.virtualWrite(V2, 0);

    //Set the full tank value
    Blynk.virtualWrite(V8, fullTankServs);

    tankEmptyNotified = false;
    outOfFood(0);

}

//1 - Turn on LED
//0 - Turn off LED

void outOfFood(int status) {
    if (!tankEmptyNotified && status == 1) {
        tankEmptyNotified = true;
        writeLog((char*)"critical", (char*)"Out of food");
        digitalWrite(redLed, 1);
    }
    Blynk.virtualWrite(V6, status);
}

void setServedMeals(int servedMealsBlynk) {
    mealsLeft = servedMealsBlynk;
}

// LEDs Blink funcs:
//  Blinking green LED{
void blinkGreen(int blinksNum)
{
    for (int b = 0; b < blinksNum; b += 1)
    {
        digitalWrite(greenLed, 1);
        delay(150);
        digitalWrite(greenLed, 0);
        delay(150);
    }
}
// Blinking yellow LED
void blinkYellow(int blinksNum)
{
    for (int b = 0; b < blinksNum; b += 1)
    {
        digitalWrite(yellowLed, 1);
        delay(150);
        digitalWrite(yellowLed, 0);
        delay(150);
    }
}
// Blinking red LED
void blinkRed(int blinksNum)
{
    for (int b = 0; b < blinksNum; b += 1)
    {
        digitalWrite(redLed, 1);
        delay(150);
        digitalWrite(redLed, 0);
        delay(150);
    }
}

// Realse Food:
void ReleaseFood()
{
    writeLog((char*)"info", (char*)"Feeding");

    mealsLeft -= 1;
    Blynk.virtualWrite(V8, mealsLeft);

    char msg[24];
    snprintf(msg, 24, "Serves Left : %d", mealsLeft);
    writeLog((char*)"info", (char*)msg);

    digitalWrite(greenLed, 1); // Turn on green LED
    Serial.println("Releasing Food! number of rounds: ");
    Serial.println(rnds);

    int roundInd;
    for (roundInd = 0; roundInd <= rnds; roundInd += 1)
    {
        //From 0° -> 180°
        for (pos = 0; pos <= 180; pos += 1) {
            servo.write(pos);
            delay(15);
        }

        delay(15);

        //From 180° -> 0°
        for (pos = 180; pos >= 0; pos -= 1) {
            servo.write(pos);
            delay(15);
        }

    }
    // check if feed rounds is over
    if (roundInd == rnds + 1)
    {
        digitalWrite(greenLed, 0); // Turn off green LED
        Serial.println("Completed Feeding");

    }

    //Set the next meal
    pendingBrk = !pendingBrk;
    pendingDnr = !pendingDnr;

    Serial.println("Start delay for 1 min after feeding");
    delay(5000); // delay in order to make sure we pass full 1 min
    Serial.println("End of delay ");

    //Change the V1 state back to 0
    Blynk.virtualWrite(V1, 0);
}

#pragma endregion Functions

#pragma region BlynkVirtualPinsWriting
/*
V0 - SchedMode + LED
V1 - Feed
V2 - Reset Tank
V3 - Breakfast Time
V4 - Dinner Time
V5 -
V6 - Out Of Food
V8 - Meals lefts
*/
BLYNK_WRITE(V0)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    Serial.print("scheduler value is: ");
    Serial.println(pinValue);

    ChangeSchedMode(pinValue);
}

BLYNK_WRITE(V1)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    Serial.print("Feed switch value is: ");
    Serial.println(pinValue);

    ReleaseFood();
}

BLYNK_WRITE(V2)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V2 to a variable
    Serial.print("Reset switch value is: ");
    Serial.println(pinValue);

    resetTank();
}

//Get Meals Left
BLYNK_WRITE(V8)
{
    int pinValue = param.asInt();
    Serial.print("Meals left value is: ");
    Serial.println(pinValue);

    setServedMeals(pinValue); // assigning incoming value from pin V8 to a variable
}
#pragma endregion BlynkVirtualPinsWriting

//When device is connected to server...
BLYNK_CONNECTED() {
    //Blynk.sendInternal("rtc", "sync"); //request current local time for device

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    Blynk.syncVirtual(V8);

    //Dont Feed
    Blynk.virtualWrite(V1, 0);

    //Sche is ON:
    Blynk.virtualWrite(V0, 1);

    writeLog((char*)"info", (char*)"Device is Online");
}

void writeLog(char eventType[], char msg[]) {
    Serial.println(msg);

    Blynk.logEvent(eventType, msg);

}

void blynkLoop(void* pvParameters) {  //task to be created by FreeRTOS and pinned to core 0
    while (true) {
        if (BLYNK_ON_CORE_0) {  //if user selected core 1, then don't blynk here -- this is only for "core 0" blynking
            Blynk.run();

        }
        vTaskDelay(random(1, 10));
    }
}

void setup() {

    Serial.begin(9600);

#pragma region setupWifiLandingPage
#define WIFI_TIMEOUT	20 // 10 seconds
    // WiFi Part
    WiFi.mode(WIFI_AP_STA);
    if (!WiFi.softAP("ESP32AP", "12345678"))
    {
        Serial.println("Failed to init WiFi AP");
    }
    else
    {
        Serial.println("IP address of AP is:");
        Serial.println((WiFi.softAPIP()));
    }
    if (!MDNS.begin("esp32"))
    {
        printf("Error setting up MDNS responder!\n");
    }
    else
        printf("mDNS responder started\n");
    MDNS.addService("http", "tcp", 80);
    // Web pages setup
    server.on("/", handle_OnConnect);
    server.on("/wifiSetupSelected", wifiSetup_page);
    server.on("/ioPageSelected", toggleIO_page); //toggleIO_page
    server.onNotFound(handle_NotFound);
    server.begin();

    spiffsOK = false;
    if (SPIFFS.begin())
    {
        Serial.println("SPIFFS mounted");
        spiffsOK = true;
    }
    else
    {
        if (SPIFFS.format())
        {
            printf("SPIFFS formated\n");
            if (SPIFFS.begin())
            {
                printf("SPIFFS mounted\n");
                spiffsOK = true;
            }
        }
    }
    if (spiffsOK == false)
    {
        printf("SPIFFS error!\n");
    }
#pragma endregion setupWifiLandingPage
}

    

//maybe needed?
String wifiName = "TPLINK", wifiPass = "";

void loop() {

#pragma region LoopWifiBlock
    int numOfClientsConnected;
    File file;
    char tempStr[32];
    bool wifiBeginCalled = false, IO3set = true;

    if (spiffsOK)


    {
        file = SPIFFS.open("/wifiSetup.txt", "r");
        if (file)
        {
            wifiName = file.readStringUntil('\n');
            printf("wifi name: %s\n", wifiName.c_str());
            wifiPass = file.readStringUntil('\n');
            printf("wifi pass: %s\n", wifiPass.c_str());
            file.close();
        }
    }

    /*if (wifiName != "")
    {
        connectToWiFiSTA(wifiName.c_str(), wifiPass.c_str());
    }*/

    while (1)
    {
        numOfClientsConnected = WiFi.softAPgetStationNum();
        if (!blynkHasConfigured && (WiFi.status() == WL_CONNECTED))
        {
            Serial.println("Blynk is now connecting");
            TestBlynk((char*)auth, (char*)wifiName.c_str(), (char*)wifiPass.c_str());
            blynkHasConfigured = true;
        }
        // If WiFi is not connected and we have a user connected to our AP, trying to connect keeps changing the WiFi channel and will cause disconnectation of the user from our AP
        if ((numOfClientsConnected > 0) && (WiFi.status() != WL_CONNECTED))
        {
            WiFi.disconnect();
            delay(1000);
            wifiBeginCalled = false;
        }
        if ((numOfClientsConnected == 0) && (WiFi.status() != WL_CONNECTED))
        {
            // only once!!
            if (!wifiBeginCalled)
            {
                WiFi.begin(wifiName.c_str(), wifiPass.c_str());
                wifiBeginCalled = true;
            }
        }
        if ((numOfClientsConnected > 0) || (WiFi.status() == WL_CONNECTED))
        {
            server.handleClient();
            if (wifiSetupPageVisited)
            {
                if (server.args() >= 2)
                { // Arguments were received
                    String ssidName = server.arg(0);
                    String ssidPassword = server.arg(1);

                    Serial.println(server.args());
                    Serial.println((const char*)ssidName.c_str());
                    Serial.println((const char*)ssidPassword.c_str());
                    wifiSetupPageVisited = false;
                    if (spiffsOK)
                    {
                        if (ssidName != wifiName || ssidPassword != wifiPass)
                        {
                            file = SPIFFS.open("/wifiSetup.txt", "w");
                            if (file)
                            {
                                sprintf(tempStr, "%s\n", ssidName.c_str());
                                file.write((const unsigned char*)tempStr, strlen(tempStr));
                                sprintf(tempStr, "%s\n", ssidPassword.c_str());
                                file.write((const unsigned char*)tempStr, strlen(tempStr));

                                file.flush();
                                file.close();
                                wifiName = ssidName;
                                wifiPass = ssidPassword;
                                WiFi.disconnect();
                                delay(1000);
                                WiFi.begin(wifiName.c_str(), wifiPass.c_str());
                            }
                        }
                    }
                }
            }
            if (toggleIOpageVisited)
            {
                toggleIOpageVisited = false;
                if (server.args() > 0)
                { // Arguments were received
                    String ioState = server.arg(0);

                    Serial.println(server.args());
                    Serial.println((const char*)ioState.c_str());
                }
            }
        }
    }
#pragma endregion LoopWifiBlock

#pragma region HandleFeedLoop

    if (schedIsActive)
    {
        // Breakfast Time
        if (pendingBrk && (brkTimeH == getHur() && brkTimeM == getMin()))
        {
            Serial.println("Breakfast is now being served according to sched");
            ReleaseFood(); // Food Timeeee
        }

        // Dinner Time
        if (pendingDnr && (dnrTimeH == getHur() && dnrTimeM == getMin()))
        {
            Serial.println("Dinner is now being served according to sched");
            ReleaseFood(); // Food Timeeee
        }
    }

    // manully relase food - bt preesed
    isDoseBtPressed = LOW; //digitalRead(buttunForFeed);
    if (isDoseBtPressed == HIGH)
    {
        Serial.println("Meal has served manually");
        ReleaseFood();
    }
    // tank is empty
    if (mealsLeft == 0)
    {
        outOfFood(1);
    }

    // reset tank - bt pressed
    isResetBtPressed = digitalRead(buttunForResetTank);
    if (isResetBtPressed == HIGH)
    {
        resetTank();
    }
}
#pragma endregion HandleFeedLoop

#pragma region htmlBLOCKandOthers
#define WIFI_TIMEOUT	20 // 10 seconds
bool connectToWiFiSTA(const char* name, const char* pass)
{
    unsigned int timeOutCounter = 0;
    WiFi.begin(name, pass);
    while ((WiFi.status() != WL_CONNECTED) && (timeOutCounter < WIFI_TIMEOUT))
    {
        delay(500);
        Serial.print(".");
        timeOutCounter++;
    }
    if (timeOutCounter != WIFI_TIMEOUT)
    {
        printf("\nWiFi connected.\n");
        Serial.println(WiFi.localIP().toString().c_str());
        Serial.println(WiFi.macAddress());
        return true;
    }
    else
    {
        printf("WiFi: cannot connect to: %s\n", name);
        return false;
    }
}

const char mainMenuPage[]PROGMEM = R"=====(
<!DOCTYPE html> 
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>ESP32 WebPage</title>
<style>
html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}
.button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 10px;}
.button-on {background-color: #1abc9c;}
.button-on:active {background-color: #16a085;}
p {font-size: 14px;color: #888;margin-bottom: 10px;}
</style>
</head>
<body>
<meta charset="utf-8">
<html lang="he">
<h1>Intel's Makers Demo Web Page</h1>
<a class="button button-on" href="/wifiSetupSelected">WiFi Setup</a>
<a class="button button-on" href="/ioPageSelected">IO Control</a>
<a class="button button-on" href="/handleTempHumidityPage">Temp</a>
</body>
<href="/">
</html>)=====";

const char setupWiFiHTMLpart1[]PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
<style>
html {font-family: Arial; display: inline-block; text-align: center;}
.button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 10px;}
.boxStyle {  padding: 12px 20px;  margin: 8px 0;  box-sizing: border-box;  border: 2px solid red;  border-radius: 10px; font-size: 20px;text-align: center;}
</style>
</head>
<body>
<form action="/" method="POST">
Access Point Name:<br>
<input type="text" class="boxStyle" name="AccessPoint" value=
)rawliteral";
//""><br>
const char setupWiFiHTMLpart2[]PROGMEM = R"rawliteral(
Password:<br>
<input type="text" class="boxStyle" name="Password" value=
)rawliteral";
//""><br>
const char setupWiFiHTMLpart3[]PROGMEM = R"rawliteral(
<input type="submit" class="button" value="OK">
</form>
</body>
<href="/">
</html>
)rawliteral";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    .button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 10px;}
  </style>
</head>
<body>
<h2>ESP Web Server</h2>
<h4>IO #1</h4>
<label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="1" checked><span class="slider"></span></label>
<h4>IO #2</h4>
<label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="2" checked><span class="slider"></span></label>
<h4>IO #3</h4>
<label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="3" checked><span class="slider"></span></label>
<h4><a class="button button-on" href="/">Home</a></h4>
<script>function toggleCheckbox(element) {
	var request = new XMLHttpRequest();
	var strOn	= "IO " + element.id + " ON";
	var strOff	= "IO " + element.id + " OFF";
	if(element.checked){ request.open("POST", "", true); request.setRequestHeader("Content-type", " ");request.send(strOn);}
	else { request.open("POST", "", true); request.setRequestHeader("Content-type", " ");request.send(strOff);}
	die();
}</script>
</body>
<href="/">
</html>
)rawliteral";

void handle_OnConnect()
{
    toggleIOpageVisited = false;
    server.send(200, "text/html", mainMenuPage);
}

void wifiSetup_page()
{
    wifiSetupPageVisited = true;
    toggleIOpageVisited = false;
    String setupWiFiHTML = setupWiFiHTMLpart1;
    //""><br>
    setupWiFiHTML += "\"";
    setupWiFiHTML += wifiName;
    setupWiFiHTML += "\"><br>";
    setupWiFiHTML += setupWiFiHTMLpart2;
    setupWiFiHTML += "\"";
    setupWiFiHTML += wifiPass;
    setupWiFiHTML += "\"><br>";
    setupWiFiHTML += setupWiFiHTMLpart3;

    server.send(200, "text/html", setupWiFiHTML);
    printf("wifiSetupPageVisited\n");
}

void toggleIO_page()
{
    toggleIOpageVisited = true;
    wifiSetupPageVisited = false;
    server.send(200, "text/html", index_html);
    printf("toggleIOpageVisited\n");
}

void handle_NotFound()
{
    server.send(404, "text/plain", "Not found");
}
#pragma endregion htmlBLOCKandOthers
