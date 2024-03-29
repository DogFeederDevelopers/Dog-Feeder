<h1 align="center">
  <br>
 <img src="https://github.com/DogFeederDevelopers/Dog-Feeder/blob/ESP32Version/Other/Logos/dogfeederLogo.png"  alt="The Dog Feeder" width="200"></a>

  <br>
  <p align="center"> <img src="https://komarev.com/ghpvc/?username=Dog-Feeder&label=Repo%20views&color=0e75b6&style=flat" alt="DogFeeder" /> </p>

# ESP32 Project - Automatic dog feeder

This project is an ESP32-based automatic dog feeder system that uses the Blynk platform for monitoring and controlling. It allows the user to schedule meal times, reset the feeding tank, and feed the dog automatically. The system also has a web dashboard and LEDs that indicate the status of the feeding tank, and a servo motor that controls the feeding mechanism.

___

## Installation

* Clone the project repository or download the source code.
* Open the source code in the Arduino IDE or any other.
* Install the required libraries (Blynk, WiFi, Servo, and SPIFFS).
* Connect the ESP32 board to the computer.
* Select the board from the Tools menu.
* Set the upload speed to 115200.
* Upload the code to the ESP32 board.
* Connect the feeding mechanism (servo motor) and LEDs to the board as indicated in the code.

## Usage
* Open an account on [Blynk](www.blynk.com).
* Create a new project in the Blynk app.
* Scan the QR code provided by the app to set up the template.
* Obtain the authentication token for the project.
* Create a new header file named `secrets.h` in the same directory as your main `.ino` file.
* In `secrets.h`, define the following constants with your actual values provided from your Blynk account:
```cpp
#define BLYNK_TEMPLATE_ID "TMP9187341"
#define BLYNK_DEVICE_NAME "DogFeederDevice"
#define BLYNK_AUTH_TOKEN "la3sjDksFGkj2342ls1Sa3Lvfd1s"
```
* Upload the modified code to the ESP32 board.
* Power on the board and wait for it to connect to the WiFi network.
* Use the Blynk app to control and monitor the dog feeder system Functionality.


## The system has the following features:

* Automatic meal scheduling: the user can set the breakfast and dinner times in the code.
* Manual feeding: the user can manually dispense food using the Blynk app or a physical button.
* Tank reset: the user can reset the feeding tank to its full capacity using the Blynk app or a physical button.
* LED indicators: the system has three LEDs that indicate the status of the feeding tank: red for empty, yellow for not scheduled, and green for full.
* Servo motor: the system uses a servo motor to dispense food.
* Control the system from the Blynk app.
* Receive notifications when the tank is empty.
* Adjust feeding time.
* Servo motor: the system uses a servo motor to dispense food.
___
## Datasreams:
| Virtual Pin  | Function         |
| --- | ---------------- |
| V0  | SchedMode + LED  |
| V1  | Feed             |
| V2  | Reset Tank       |
| V3  | Dinner Time      |
| V4  | Breakfast Time   |
| V5  | Feed Delay Time  |
| V6  | Out Of Food      |
| V7  |                  |
| V8  | Meals lefts      |



## Examples

<table align="center">
  <tr>
    <td><img src="https://github.com/DogFeederDevelopers/Dog-Feeder/blob/ESP32Version/Other/Logos/app-demo.jpeg" width="200"></td>
    <td><img src="https://github.com/DogFeederDevelopers/Dog-Feeder/blob/ESP32Version/Other/Logos/ModelShot.png" width="200"></td>
  </tr>
</table>


