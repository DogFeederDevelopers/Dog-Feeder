#include <Servo.h>

//Time Variables:
double elapsedHrs;
double fedHrsAgo = 0;
const double feedInterval = 12.0;

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
  pinMode(buttunForFeed, INPUT);
  pinMode(ledRedNoFood, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreenRunning, OUTPUT);
  blink(2);
  Serial.begin(9600);
}

void loop() {
    elapsedHrs = (millis()/1000);
   if((elapsedHrs-fedHrsAgo) >= feedInterval ){
      fedHrsAgo = elapsedHrs;
      ReleaseFood();
  }  
  //manully relase food - bt preesed 
  isDoseBtPressed = digitalRead(buttunForFeed);
  if(isDoseBtPressed  == HIGH){       
    ReleaseFood();
    delay(1000);
  } 
}

////////////////////////////
//Functions:


int blink(int blinkAmount){
  for(int b = 0; b < blinkAmount ; b += 1){
    digitalWrite(ledGreenRunning,1);
    delay(150);
    digitalWrite(ledGreenRunning,0);
    delay(150);
  }
}

//Realse Food:
boolean ReleaseFood(){
  Serial.println("bt preesd - start feed");
    digitalWrite(ledGreenRunning,1);
  int roundInd;
  for(roundInd = 0 ; roundInd <= rnds; roundInd += 1){
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
  Serial.println("roundInd:");
  Serial.println(roundInd);
  
  Serial.println("rnds:");
  Serial.println(rnds);
  if(roundInd== rnds+1){
    Serial.println("bt preesd - start feed");
    digitalWrite(ledGreenRunning,0);
    return true;
  }
  else{
    return false;
  }
}
