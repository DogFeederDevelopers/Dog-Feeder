#include <Servo.h>
#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);


//Time Variables:
int brkTimeH = 7;
int brkTimeM = 30;

int dnrTimeH = 19;
int dnrTimeM = 30;

bool pendingBrk;
bool pendingDnr;

//Feed Rounds:
int pos = 0;    // variable to store the servo position
int rnds = 1;    //The amount of moves for 1 dose
int servedMeals =0; 
int fullTankServs = 4; //The amount of rounds for full container

//Buttons:
int isDoseBtPressed  = 0; //Flag for manually pressing for dose
int isResetBtPressed  = 0; //Flag for pressing to reset
#define buttunForFeed 2 //Button for manully feed action
#define buttunForResetTank 3 //Button for resetting Tank + long press will turn on/off auto-Scheduale
//LEDs:
#define ledRedNoFood 50 //Red light - No Food
#define ledYellow 51 //Yellow light - auto-schedule is off
#define ledGreenRunning 52 //Green light - System init blinking, feeding

//Servo:
Servo myservo;  // create servo object to control a servo
#define servoAttachPin 9

void setup() {
  myservo.attach(servoAttachPin);
  myservo.write(0); // set servo to 0° postion
  
  pendingBrk = true;
  pendingDnr = false;

  pinMode(buttunForFeed, INPUT);
  pinMode(buttunForResetTank, INPUT);
  pinMode(ledRedNoFood, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreenRunning, OUTPUT);

  blink(2,'G');
  
  Serial.begin(9600); //For all other 
  //Serial.begin(115200); //For RTC

  rtc.begin();// Initialize the rtc object

    // The following lines can be uncommented to set the date and time
    //  rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
    //  rtc.setTime(23, 12, 0);     // Set the time to 12:00:00 (24hr format)
    //  rtc.setDate(6, 8, 2022);   // Set the date to January 1st, 2014
}

void loop() {
  //Sched Region
  //Breakfast Time
  if(pendingBrk && (brkTimeH == getHur() && brkTimeM == getMin())){
    pendingBrk = false;
    pendingDnr = true;    

    ReleaseFood(); //Food Timeeee
    delay(60000); //delay in order to make sure we pass full 1 min
  }

  //Dinner Time  
  if(pendingDnr && (dnrTimeH == getHur() && dnrTimeM == getMin())){
    pendingDnr = false;
    pendingBrk = true;
 
    ReleaseFood(); //Food Timeeee
    delay(60000); //delay in order to make sure we pass full 1 min
  }
  //End_Sched Region

  //manully relase food - bt preesed
  isDoseBtPressed = digitalRead(buttunForFeed);
  if (isDoseBtPressed  == HIGH) {
    pendingBrk = !pendingBrk;
    pendingDnr = !pendingDnr;

    ReleaseFood();
    delay(1000);
  }

  //reset tank - bt pressed
  isResetBtPressed = digitalRead(buttunForResetTank);
  if (isResetBtPressed == HIGH) {
    servedMeals = 0;
    delay(1000);
  }
  if(servedMeals == fullTankServs){
    digitalWrite(ledRedNoFood, 1);
  }
}

////////////////////////////////////////////////////////

//Functions:
void blink(int blinkAmount ) {
  for (int b = 0; b < blinkAmount ; b += 1) {
    digitalWrite(ledGreenRunning, 1);
    delay(150);
    digitalWrite(ledGreenRunning, 0);
    delay(150);
    }
  }
void resetTank(){
  servedMeals = 0;
  digitalWrite(ledRedNoFood, 0);
}
//Realse Food:
boolean ReleaseFood() {
  Serial.print("Servs Left:");
  Serial.print(fullTankServs-servedMeals);
  servedMeals += 1;
  digitalWrite(ledGreenRunning, 1); //Turn on green LED
  int roundInd;
  for (roundInd = 0 ; roundInd <= rnds; roundInd += 1) {
    for (pos = 0; pos <= 180; pos += 5) { // goes from 0° to 180°
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
      //Serial.println(pos);
    }
    delay(10);
    for (pos = 180; pos >= 0; pos -= 5) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
      //Serial.println(pos);

    }
  }


  if (roundInd == rnds + 1) {
    Serial.println("bt preesd - start feed");
    digitalWrite(ledGreenRunning, 0);
    return true;
  }
  else {
    return false;
  }

}

//Get min
int getMin(){
  return rtc.getTime().min;    
}

//Get hour
int getHur(){
  return rtc.getTime().hour;    
}

//Get second
int getSec(){
  return rtc.getTime().sec;    
}
