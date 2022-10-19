#include <WiFi.h>
#include "time.h"
#include <Servo.h>

// Varibales:
// Wifi:
const char* ssid = "Nova-2GHz";
const char* password = "0586669888Nova";

// Time Variables:
int brkTimeH = 7;  
int brkTimeM = 30; 

int dnrTimeH = 17; 
int dnrTimeM = 31; 

bool schedIsActive;
bool pendingBrk;
bool pendingDnr;

// NTP server: 
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

// Feed Rounds:
int pos = 0;  // variable to store the servo position
int rnds = 5; // The amount of moves for 1 dose ////////////////////////////////////////////////////////////
int servedMeals = 0;
const int fullTankServs = 4; // The amount of rounds for full container////////////////////////////////////////////////////////////

// Buttons:
int isDoseBtPressed = 0;     // Flag for manually pressing for dose
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

void setup()
{
    Serial.begin(115200);

    // Connect to Wi-Fi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");

    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printInitTime();

    //Servo setup
    servo.attach(servoAttachPin);

    schedIsActive = true;

    //LEDs and buttuns
    pinMode(buttunForFeed, INPUT);
    pinMode(buttunForResetTank, INPUT);
    pinMode(redLed, OUTPUT);
    pinMode(yellowLed, OUTPUT);
    pinMode(greenLed, OUTPUT);

    blinkGreen(2);
    setNextMeal();
}

void loop()
{
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
    isDoseBtPressed = digitalRead(buttunForFeed);
    if (isDoseBtPressed == HIGH)
    {
        Serial.println("Meal has served manually");
        ReleaseFood();
    }
    // tank is empty
    if (servedMeals == fullTankServs)
    {
        Serial.println("Tank is empty");
        digitalWrite(redLed, 1);
    }

    // reset tank - bt pressed
    isResetBtPressed = digitalRead(buttunForResetTank);
    if (isResetBtPressed == HIGH)
    {
        delay(2000);
        isResetBtPressed = digitalRead(buttunForResetTank);
        if (isResetBtPressed == HIGH)
        {
            Serial.println("Long prees - change schedule mode");
            blinkYellow(2);
            ChangeSchedMode();
        }
        else
        {
            Serial.println("Short prees - reset tank");
            resetTank();
        }
        delay(2000);
    }
}

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
        // fix that!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

void ChangeSchedMode()
{
    if (schedIsActive)
    {
        Serial.println("Turns off sched and consistently turn on yellow light");
        schedIsActive = false;
        digitalWrite(yellowLed, 1);
    }
    else
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
    delay(60000); // delay in order to make sure we pass full 1 min
}
