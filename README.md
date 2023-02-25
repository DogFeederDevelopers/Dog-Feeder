<h1 align="center">
  <br>
 <img src="https://github.com/AlmogShKt/Dog-Feeder/blob/ESP32Version/Other/Logos/dogfeederLogo.png"  alt="The Dog Feeder" width="200"></a>
  
  <br>



# ESP32 Project - Automatic dog feeder 


This project is an ESP32-based automatic dog feeder system that uses the Blynk platform for monitoring and controlling. It allows the user to schedule meal times, reset the feeding tank, and feed the dog manually. The system also has LEDs that indicate the status of the feeding tank, and a servo motor that controls the feeding mechanism.
___ 

## Installation

Clone the project repository or download the source code.
Open the source code in the Arduino IDE.
Install the required libraries (Blynk, WiFi, Servo, and SPIFFS).
Connect the ESP32 board to the computer.
Select the board from the Tools menu.
Set the upload speed to 115200.
Upload the code to the ESP32 board.
Connect the feeding mechanism (servo motor) and LEDs to the board as indicated in the code.

___ 

## Usage

Create a new project in the Blynk app.
Scan the QR code provided by the app to set up the template.
Obtain the authentication token for the project.
Open the source code and set the BLYNK_AUTH_TOKEN variable to the authentication token obtained in the previous step.
Optionally, set the WiFi SSID and password in the ssid and pass variables.
Upload the modified code to the ESP32 board.
Power on the board and wait for it to connect to the WiFi network.
Use the Blynk app to control and monitor the dog feeder system.
Functionality

___ 
## The system has the following features:

* Automatic meal scheduling: the user can set the breakfast and dinner times in the code.
* Manual feeding: the user can manually dispense food using the Blynk app or a physical button.
* Tank reset: the user can reset the feeding tank to its full capacity using the Blynk app or a physical button.
* LED indicators: the system has three LEDs that indicate the status of the feeding tank: red for empty, yellow for not scheduled, and green for full.
* Servo motor: the system uses a servo motor to dispense food.
* Control the system from Blynk app.
* Recive notofication when the tanks is empty. 
* Adjust feeding time. 
* Servo motor: the system uses a servo motor to dispense food.

