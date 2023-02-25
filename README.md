<h1 align="center">
  <br>
  <img src="https://github.com/AlmogShKt/Dog-Feeder/blob/0b3d9ad9c5fee7eb1dcdea9aadd57244f0dfef9d/Other/logo.png" alt="The Dog Feeder" width="200"></a>
  
  <br>
  <br>

<img src="https://github.com/AlmogShKt/Dog-Feeder/blob/00357df0f746faa0449a867afbfe5ed29b5a4f23/Other/Logos/logo2.png" alt="The Dog Feeder" alt="The Dog Feeder" width="200"></a>
<br>
  <br>
</h1>
Arduino Project - Automatic dog feeder 
<h1>

</h1>
This project is an ESP32-based automatic dog feeder system that uses the Blynk platform for monitoring and controlling. It allows the user to schedule meal times, reset the feeding tank, and feed the dog manually. The system also has LEDs that indicate the status of the feeding tank, and a servo motor that controls the feeding mechanism.

Installation

Clone the project repository or download the source code.
Open the source code in the Arduino IDE.
Install the required libraries (Blynk, WiFi, Servo, and SPIFFS).
Connect the ESP32 board to the computer.
Select the board from the Tools menu.
Set the upload speed to 115200.
Upload the code to the ESP32 board.
Connect the feeding mechanism (servo motor) and LEDs to the board as indicated in the code.
Usage

Create a new project in the Blynk app.
Scan the QR code provided by the app to set up the template.
Obtain the authentication token for the project.
Open the source code and set the BLYNK_AUTH_TOKEN variable to the authentication token obtained in the previous step.
Optionally, set the WiFi SSID and password in the ssid and pass variables.
Upload the modified code to the ESP32 board.
Power on the board and wait for it to connect to the WiFi network.
Use the Blynk app to control and monitor the dog feeder system.
Functionality

The system has the following features:

Automatic meal scheduling: the user can set the breakfast and dinner times in the code.
Manual feeding: the user can manually dispense food using the Blynk app or a physical button.
Tank reset: the user can reset the feeding tank to its full capacity using the Blynk app or a physical button.
LED indicators: the system has three LEDs that indicate the status of the feeding tank: red for empty, yellow for not scheduled, and green for full.
Servo motor: the system uses a servo motor to dispense food.
Contributing

Contributions are welcome! Please create a pull request with any changes or bug fixes.

License

This project is licensed under the MIT License. See the LICENSE file for details.
