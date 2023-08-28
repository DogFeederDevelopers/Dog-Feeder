// Google NTP server:
const char* ntpServer = "time.google.com";
// Israel TimeZone:
const long gmtOffset_sec = 7200;
const int daylightOffset_sec = 3600;

// Deafults or init values:
const int initBrkTimeH = 7;
const int initBrkTimeM = 30;
const int initDnrTimeH = 17;
const int initDnrTimeM = 30;

const int initServoRounds = 5; // The amount of moves for 1 dose
const int initServoPosition = 0; // variable to store the servo position
const int initFullTankMeals = 4; // The initial amount of meals per full tank
const int initFeedDealyTime = 60000; // The initial feed delay time in miliseconds

// Pins:
const int buttunForFeed = 23;
const int buttunForResetTank = 22;
const int redLed = 12;
const int yellowLed = 14;
const int greenLed = 27;
const int servoAttachPin = 32;

bool BLYNK_ON_CORE_0 = true;