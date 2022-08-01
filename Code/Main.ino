#include <Servo.h>

Servo myservo;  // create servo object to control a servo

//Feed rounds:
int pos = 0;    // variable to store the servo position
int cir = 1;    //The amount of moves for 1 serve
int i = cir + 1; //set the def value for each round
int fullTankRounds = 12; //The

//Buttons:
int manullyButtunForFeed = 8; //Button for manully feed action

//LED:
int ledGreenRunning = 7; //Green light - is on while: initition of the system, feeding
int ledNoFood =0; //Red lught - is on when the food is over.

//??
int isMuPressed = 0;


int blink(int blinkamount){
  for(int b = 0; b < blinkamount ; b += 1){
    digitalWrite(ledGreenRunning,1);
    delay(150);
    digitalWrite(ledGreenRunning,0);
    delay(150);
  }
}

void setup() {
  myservo.attach(9);
  myservo.write(0);              // tell servo to go to position in variable 'pos'

  pinMode(manullyButtunForFeed, INPUT);
  pinMode(ledGreenRunning, OUTPUT);
  blink(2);
  Serial.begin(9600);
}

void loop() {
  //set i for 0 - time for food

  //manully relase food - bt preesed 
  isMuPressed = digitalRead(manullyButtunForFeed);
  if(isMuPressed == HIGH){
    i = 0;
    Serial.println("bt preesd - start feed");
    digitalWrite(ledGreenRunning,1);
    delay(1000);

  } 
  



  //realse food
  for(; i <= cir; i += 1){
    Serial.println(i);
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
      //Serial.println(pos);

    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
      //Serial.println(pos);

    }
   



  }
  if(i == cir+1){
    Serial.println("bt preesd - start feed");
    digitalWrite(ledGreenRunning,LOW);

  }

}
