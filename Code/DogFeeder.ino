#include <Servo.h>
#include <DS3231.h>

//Varibales:

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

//For blink function, will hold the pin number
uint8_t BlinkPinLED;

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
int rnds4fullTnk = 12; //The amount of round for full container

//Buttons:
int isDoseBtPressed  = 0; //Flag for manually pressing for dose
#define buttunForFeed 8 //Button for manully feed action
//LEDs:
#define ledRedNoFood 5 //Red light - No Food
#define ledYellow 6 //Yello light - 
#define ledGreenRunning 7 //Green light - System init blinking, feeding

//Servo:
Servo myservo;  // create servo object to control a servo
#define servoAttachPin 9

void setup() {
  myservo.attach(servoAttachPin);
  myservo.write(0); // set servo to 0° postion
  
  pendingBrk = true;
  pendingDnr = false;

  pinMode(buttunForFeed, INPUT);
  pinMode(ledRedNoFood, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreenRunning, OUTPUT);

  blink(2,'R');
  
  Serial.begin(9600); //For all other 
  //Serial.begin(115200); //For RTC

  rtc.begin();// Initialize the rtc object

    // The following lines can be uncommented to set the date and time
//  rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
//  rtc.setTime(23, 12, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(6, 8, 2022);   // Set the date to January 1st, 2014
}

void loop() {
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
   
  //manully relase food - bt preesed
  isDoseBtPressed = digitalRead(buttunForFeed);
  if (isDoseBtPressed  == HIGH) {
    pendingBrk = !pendingBrk;
    pendingDnr = !pendingDnr;

    ReleaseFood();
    delay(1000);
  }
}

/* --------------- */
//Functions:

//Blinking green LED
void blink(int blinkAmount, char ColorLED) {
  //For blinking with mulipule colors (red&green)
  
  switch (ColorLED) {
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
  for (int b = 0; b < blinkAmount ; b += 1) {
    digitalWrite(BlinkPinLED, 1);
    delay(150);
    digitalWrite(BlinkPinLED, 0);
    delay(150);
  }
}

//For blinking with mulipule colors (red&green)
void blink(int blinkAmount, char ColorLED, bool multiColor) {
  if(multiColor){
    for (int b = 0; b < blinkAmount ; b += 1) {
    digitalWrite(ledRedNoFood, 1);
    digitalWrite(ledGreenRunning, 1);
    delay(150);
    digitalWrite(ledRedNoFood, 0);
    digitalWrite(ledGreenRunning, 0);
    delay(150);
    }
  } 
}
  




//Realse Food:
boolean ReleaseFood() {
  digitalWrite(ledGreenRunning, 1); //Turn on green LED
  int roundInd;
  for (roundInd = 0 ; roundInd <= rnds; roundInd += 1) {
    for (pos = 0; pos <= 180; pos += 5) { // goes from 0° to 180°
      // in steps of 1 degree
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


