#include <DS3231.h>
#include <Servo.h>

// Init the DS3231 using the hardware interface
DS3231 rtc(SDA, SCL);

// Varibales:
// Time Variables:
int brkTimeH = 7;  ////////////////////////////////////////////////////////////
int brkTimeM = 30; ////////////////////////////////////////////////////////////

int dnrTimeH = 19; ////////////////////////////////////////////////////////////
int dnrTimeM = 30; ////////////////////////////////////////////////////////////

bool schedIsActive;
bool pendingBrk;
bool pendingDnr;

// Feed Rounds:
int pos = 0;  // variable to store the servo position
int rnds = 1; // The amount of moves for 1 dose ////////////////////////////////////////////////////////////
int servedMeals = 0;
int fullTankServs = 4; // The amount of rounds for full container////////////////////////////////////////////////////////////

// Buttons:
int isDoseBtPressed = 0;     // Flag for manually pressing for dose
int isResetBtPressed = 0;    // Flag for pressing to reset
#define buttunForFeed 2      // Button for manully feed action
#define buttunForResetTank 3 // Button for resetting Tank + long press will turn on/off auto-Scheduale

// LEDs:
#define ledRed 50    // Red light - No Food
#define ledYellow 51 // Yellow light - auto-schedule is off
#define ledGreen 52  // Green light - System init blinking, feeding

// Servo:
Servo myservo; // create servo object to control a servo
#define servoAttachPin 9

void setup()
{
  myservo.attach(servoAttachPin);
  myservo.write(0); // set servo to 0° postion

  schedIsActive = true;

  pinMode(buttunForFeed, INPUT);
  pinMode(buttunForResetTank, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  digitalWrite(ledYellow, 0); //////////////////////////////////////////////////////////////

  Serial.begin(9600); // For all other
  // Serial.begin(115200); //For RTC

  rtc.begin(); // Initialize the rtc object
  blinkGreen(2);
  setNextMeal();
  // The following lines can be uncommented to set the date and time
  //  rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
  //  rtc.setTime(23, 12, 0);     // Set the time to 12:00:00 (24hr format)
  //  rtc.setDate(6, 8, 2022);   // Set the date to January 1st, 2014
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
    digitalWrite(ledRed, 1);
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

// Get hour
int getHur()
{
  return rtc.getTime().hour;
}
// Get min
int getMin()
{
  return rtc.getTime().min;
}
// Get second
int getSec()
{
  return rtc.getTime().sec;
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
    digitalWrite(ledYellow, 1);
  }
  else
  {
    Serial.println("Turns back on sched and turn off yellow light");
    schedIsActive = true;
    setNextMeal();
    digitalWrite(ledYellow, 0);
  }
}

void resetTank()
{
  Serial.println("Tank was resetted");
  servedMeals = 0;
  digitalWrite(ledRed, LOW);
}

// Blink funcs:
//  Blinking green LED
void blinkGreen(int blinksNum)
{
  for (int b = 0; b < blinksNum; b += 1)
  {
    digitalWrite(ledGreen, 1);
    delay(150);
    digitalWrite(ledGreen, 0);
    delay(150);
  }
}
// Blinking yellow LED
void blinkYellow(int blinksNum)
{
  for (int b = 0; b < blinksNum; b += 1)
  {
    digitalWrite(ledYellow, 1);
    delay(150);
    digitalWrite(ledYellow, 0);
    delay(150);
  }
}
// Blinking red LED
void blinkRed(int blinksNum)
{
  for (int b = 0; b < blinksNum; b += 1)
  {
    digitalWrite(ledRed, 1);
    delay(150);
    digitalWrite(ledRed, 0);
    delay(150);
  }
}

// Realse Food:
void ReleaseFood()
{
  servedMeals += 1;

  Serial.print("Serves Left: ");
  Serial.println(fullTankServs - servedMeals);

  digitalWrite(ledGreen, 1); // Turn on green LED
  Serial.println("Releasing Food! number of rounds: ");
  Serial.println(rnds);
  int roundInd;
  for (roundInd = 0; roundInd <= rnds; roundInd += 1)
  {
    for (pos = 0; pos <= 180; pos += 5)
    { // goes from 0° to 180°
      // in steps of 1 degree
      myservo.write(pos); // tell servo to go to position in variable 'pos'
      delay(15);          // waits 15 ms for the servo to reach the position
      // Serial.println(pos);
    }
    delay(10);
    for (pos = 180; pos >= 0; pos -= 5)
    {                     // goes from 180 degrees to 0 degrees
      myservo.write(pos); // tell servo to go to position in variable 'pos'
      delay(15);          // waits 15 ms for the servo to reach the position
      // Serial.println(pos);
    }
  }
  // check if feed rounds is over
  if (roundInd == rnds + 1)
  {
    digitalWrite(ledGreen, 0); // Turn off green LED
    Serial.println("Completed Feeding");
  }

  pendingBrk = !pendingBrk;
  pendingDnr = !pendingDnr;
  delay(60000); // delay in order to make sure we pass full 1 min
}