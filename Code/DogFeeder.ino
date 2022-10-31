#define BLYNK_TEMPLATE_ID           "TMPLvlo1BP8g"
#define BLYNK_DEVICE_NAME           "DogFeederTemplate"
#define BLYNK_AUTH_TOKEN            "nSgd5nnVgfpmcTq2zCp5MT3FxwnaLfaH"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "time.h"
#include <Servo.h>

//Blynk setup
char ssid[] = "Nova-2GHz";
char pass[] = "0586669888Nova";

char auth[] = BLYNK_AUTH_TOKEN;


// Time Variables:
int brkTimeH = 7;
int brkTimeM = 30;

int dnrTimeH = 17;
int dnrTimeM = 31;

bool schedIsActive;
bool pendingBrk;
bool pendingDnr;

// NTP server: 
const char* ntpServer = "il.pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 7200;

// Feed Rounds:
int pos = 0;  // variable to store the servo position
int rnds = 5; // The amount of moves for 1 dose ////////////////////////////////////////////////////////////
int servedMeals = 0;
const int fullTankServs = 4; // The amount of rounds for full container////////////////////////////////////////////////////////////

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

void printInitTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

int getSec() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return 0;
    }
    return (int)timeinfo.tm_sec;
}

int getMin() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return 0;
    }
    return (int)timeinfo.tm_min;
}

int getHur() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return 0;
    }
    return (int)timeinfo.tm_hour;
}

void setNextMeal() // sets the pending breakfast and dinner according to the current time and retunr
{
    Serial.println("time right now:");
    Serial.print(getHur());
    Serial.print(":");
    Serial.println(getMin());

    if (((getHur() > 0) && (getHur() < brkTimeH)) || ((getHur() > dnrTimeH) && (getHur() <= 23)))
    {
        // fix that!!
    }

    if ((getHur() <= brkTimeH) && (getMin() < brkTimeM))
    {
        Serial.println("Next meal is set to Breakfast");
        pendingBrk = true;
        pendingDnr = false;
    }
    else
    {
        Serial.println("Next meal is set to Dinner");
        pendingBrk = false;
        pendingDnr = true;
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
    Serial.println("Tank was resetted");
    servedMeals = 0;
    digitalWrite(redLed, LOW);

    Blynk.virtualWrite(V2, 0);
}

// Blink funcs:
//  Blinking green LED
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
    servedMeals += 1;

    Serial.print("Serves Left: ");
    Serial.println(fullTankServs - servedMeals);

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
    Serial.println("End ofdelay ");
    Blynk.virtualWrite(V1, 0);
}


#pragma endregion Functions


BLYNK_WRITE(V0)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    // You can also use:
    // String i = param.asStr();
    // double d = param.asDouble();
    Serial.print("V0 value is: ");
    Serial.println(pinValue);
    ChangeSchedMode(pinValue);
}

BLYNK_WRITE(V1)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    // You can also use:
    // String i = param.asStr();
    // double d = param.asDouble();
    Serial.print("V1 value is: ");
    
    Serial.println(pinValue);
    ReleaseFood();
}


BLYNK_WRITE(V2)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    // You can also use:
    // String i = param.asStr();
    // double d = param.asDouble();
    Serial.print("V2 value is: ");
    Serial.println(pinValue);
    resetTank();
}

BLYNK_CONNECTED() {                  //When device is connected to server...
    //Blynk.sendInternal("rtc", "sync"); //request current local time for device
    
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    //Dont Feed
    Blynk.virtualWrite(V1, 0);

    //Sche is ON:
    Blynk.virtualWrite(V0, 1);

    
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

    Blynk.begin(auth, ssid, pass);
    while (Blynk.connected() == false) {
    }
    //Servo setup
    servo.attach(servoAttachPin);

   


    //LEDs and buttuns
    pinMode(buttunForFeed, INPUT);
    pinMode(buttunForResetTank, INPUT);
    pinMode(redLed, OUTPUT);
    pinMode(yellowLed, OUTPUT);
    pinMode(greenLed, OUTPUT);

    //Blynk.syncVirtual(V2); //retrieve last blynk core pinning selection

    //this is where we start the Blynk.run() loop pinned to core 0, given priority "1" (which gives it thread priority over "0")
    xTaskCreatePinnedToCore(
        blynkLoop,      /* Function to implement the task */
        "blynk core 0", /* Name of the task */
        100000,         /* Stack size in words */
        NULL,           /* Task input parameter */
        1,              /* Priority of the task */
        NULL,           /* Task handle. */
        0);             /* Core where the task should run */

    Serial.println("");
    setNextMeal();
}

void loop() {
          
}
