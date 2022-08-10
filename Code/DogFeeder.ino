#include <DS3231.h>
#include <Servo.h>

// Varibales:

// Init the DS3231 using the hardware interface
DS3231 rtc(SDA, SCL);

// For blink function, will hold the pin number
uint8_t BlinkPinLED;

// Time Variables:
int brkTimeH = 7;
int brkTimeM = 30;

int dnrTimeH = 19;
int dnrTimeM = 30;

bool schedIsActive;
bool pendingBrk;
bool pendingDnr;

// Feed Rounds:
int pos = 0;  // variable to store the servo position
int rnds = 1; // The amount of moves for 1 dose
int servedMeals = 0;
int fullTankServs = 4; // The amount of rounds for full container

// Buttons:
int isDoseBtPressed = 0;     // Flag for manually pressing for dose
int isResetBtPressed = 0;    // Flag for pressing to reset
#define buttunForFeed 2      // Button for manully feed action
#define buttunForResetTank 3 // Button for resetting Tank + long press will turn on/off auto-Scheduale

// LEDs:
#define ledRedNoFood 50    // Red light - No Food
#define ledYellow 51       // Yellow light - auto-schedule is off
#define ledGreenRunning 52 // Green light - System init blinking, feeding

// Servo:
Servo myservo; // create servo object to control a servo
#define servoAttachPin 9

void setup()
{
  myservo.attach(servoAttachPin);
  myservo.write(0); // set servo to 0° postion

  bool schedIsActive = true;

  pinMode(buttunForFeed, INPUT);
  pinMode(buttunForResetTank, INPUT);
  pinMode(ledRedNoFood, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreenRunning, OUTPUT);

  digitalWrite(ledYellow, 0);
  blink(2, 'G');
  setNextMeal();

  Serial.begin(9600); // For all other
  // Serial.begin(115200); //For RTC

  rtc.begin(); // Initialize the rtc object

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
      pendingBrk = false;
      pendingDnr = true;

      ReleaseFood(); // Food Timeeee
      delay(60000);  // delay in order to make sure we pass full 1 min
    }

    // Dinner Time
    if (pendingDnr && (dnrTimeH == getHur() && dnrTimeM == getMin()))
    {
      pendingDnr = false;
      pendingBrk = true;

      ReleaseFood(); // Food Timeeee
      delay(60000);  // delay in order to make sure we pass full 1 min
    }
  }

  // manully relase food - bt preesed
  isDoseBtPressed = digitalRead(buttunForFeed);
  if (isDoseBtPressed == HIGH)
  {
    Serial.println("Meal has served");
    pendingBrk = !pendingBrk;
    pendingDnr = !pendingDnr;

    ReleaseFood();
    delay(1000);
  }
  // tank is empty
  if (servedMeals == fullTankServs)
  {
    digitalWrite(ledRedNoFood, 1);
  }

  // reset tank - bt pressed
  isResetBtPressed = digitalRead(buttunForResetTank);
  if (isResetBtPressed == HIGH)
  {
    delay(2000);
    isResetBtPressed = digitalRead(buttunForResetTank);
    if (isResetBtPressed == 1)
    {
      Serial.println("Long prees");
      blink(2, 'Y');
      ChangeSchedMode();
    }
    else
    {
      Serial.println("Short prees");
      resetTank();
    }
  }
}

/* --------------- */
// Functions:
void setNextMeal() // sets the pending breakfast and dinner according to the current time and retunr
{
  if ((GetHur() >= brkTimeH && GetMin() > brkTimeM))
  {
    pendingBrk = false;
    pendingDnr = true;
  }
  else
  {
    pendingBrk = true;
    pendingDnr = false;
  }
}

void ChangeSchedMode()
{
  if (schedIsActive)
  {
    // turn off sched and consistently turn on yellow light
    schedIsActive = false;
    digitalWrite(ledYellow, 1);
  }
  else
  {
    // turn back on sched and turn off yellow light
    schedIsActive = true;
    setNextMeal();
    digitalWrite(ledYellow, 0);
  }
}

void resetTank()
{
  Serial.println("Tank was resetted");
  servedMeals = 0;
  digitalWrite(ledRedNoFood, 0);
}
// Blinking green LED
void blink(int blinkAmount, char ColorLED)
{
  // For blinking with mulipule colors (red&green)

  switch (ColorLED)
  {
  case 'G':
    BlinkPinLED = "ledGreenRunning";
    break;
  case 'R':
    BlinkPinLED = "ledRedNoFood";
    break;
  case 'Y':
    BlinkPinLED = "ledYellow";
    break;
  }
  for (int b = 0; b < blinkAmount; b += 1)
  {
    digitalWrite(BlinkPinLED, 1);
    delay(150);
    digitalWrite(BlinkPinLED, 0);
    delay(150);
  }
}

// For blinking with mulipule colors (red&green)
void blink(int blinkAmount, char ColorLED, bool multiColor)
{
  if (multiColor)
  {
    for (int b = 0; b < blinkAmount; b += 1)
    {
      digitalWrite(ledRedNoFood, 1);
      digitalWrite(ledGreenRunning, 1);
      delay(150);
      digitalWrite(ledRedNoFood, 0);
      digitalWrite(ledGreenRunning, 0);
      delay(150);
    }
  }
}

// Realse Food:
boolean ReleaseFood()
{
  servedMeals += 1;

  Serial.print("Serves Left: ");
  Serial.println(fullTankServs - servedMeals);

  digitalWrite(ledGreenRunning, 1); // Turn on green LED
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

  if (roundInd == rnds + 1)
  {
    digitalWrite(ledGreenRunning, 0);
    return true;
  }
  else
  {
    return false;
  }
}

// Get min
int getMin()
{
  return rtc.getTime().min;
}

// Get hour
int getHur()
{
  return rtc.getTime().hour;
}

// Get second
int getSec()
{
  return rtc.getTime().sec;
}
