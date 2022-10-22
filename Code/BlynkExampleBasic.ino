#define BLYNK_TEMPLATE_ID           "TMPLvlo1BP8g"
#define BLYNK_DEVICE_NAME           "DogFeederTemplate"
#define BLYNK_AUTH_TOKEN            "nSgd5nnVgfpmcTq2zCp5MT3FxwnaLfaH"


#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Nova-2GHz";
char pass[] = "0586669888Nova";


void ledOnOff(int value) {
	digitalWrite(23, value);
}

void ledOnOff2(int value) {
	digitalWrite(22, value);
}

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V1)
{
	int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
	// You can also use:
	// String i = param.asStr();
	// double d = param.asDouble();
	Serial.print("V1 Slider value is: ");
	Serial.println(pinValue);
	ledOnOff(pinValue);
}

BLYNK_WRITE(V2)
{
	int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
	// You can also use:
	// String i = param.asStr();
	// double d = param.asDouble();
	Serial.print("V2 Slider value is: ");
	Serial.println(pinValue);
	ledOnOff2(pinValue);
}

void setup()
{
	// Debug console
	Serial.begin(115200);

	Blynk.begin(auth, ssid, pass);

	pinMode(22, OUTPUT);
	pinMode(23, OUTPUT);
	
}

void loop()
{
	Blynk.run();
}



