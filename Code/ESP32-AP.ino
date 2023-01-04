/*
 Author:Almog Shtaigman, Tamir Zitman, thanks to:	giltal
*/

#include <Wire.h>
#include "SPIFFS.h" 


bool spiffsOK = false;

// WiFi part
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>

WebServer server(80);
void handle_OnConnect();
void wifiSetup_page();
void toggleIO_page();
void handle_NotFound();


bool wifiSetupPageVisited = false;
bool toggleIOpageVisited = false;


// the setup function runs once when you press reset or power the board
void setup()
{
	// Setup the IOs
	Serial.begin(115200);


#define WIFI_TIMEOUT	20 // 10 seconds
	// WiFi Part
	WiFi.mode(WIFI_AP_STA);
	if (!WiFi.softAP("ESP32AP", "12345678"))
	{
		Serial.println("Failed to init WiFi AP");
	}
	else
	{
		Serial.println("IP address of AP is:");
		Serial.println((WiFi.softAPIP()));
	}
	if (!MDNS.begin("esp32"))
	{
		printf("Error setting up MDNS responder!\n");
	}
	else
		printf("mDNS responder started\n");
	MDNS.addService("http", "tcp", 80);
	// Web pages setup
	server.on("/", handle_OnConnect);
	server.on("/wifiSetupSelected", wifiSetup_page);
	server.on("/ioPageSelected", toggleIO_page); //toggleIO_page
	server.onNotFound(handle_NotFound);
	server.begin();

	spiffsOK = false;
	if (SPIFFS.begin())
	{
		Serial.println("SPIFFS mounted");
		spiffsOK = true;
	}
	else
	{
		if (SPIFFS.format())
		{
			printf("SPIFFS formated\n");
			if (SPIFFS.begin())
			{
				printf("SPIFFS mounted\n");
				spiffsOK = true;
			}
		}
	}
	if (spiffsOK == false)
	{
		printf("SPIFFS error!\n");
	}
}

// the loop function runs over and over again until power down or reset

String wifiName = "TPLINK", wifiPass = "";

void loop()
{
	int numOfClientsConnected;
	File file;
	char tempStr[32];
	bool wifiBeginCalled = false, IO3set = true;

	if (spiffsOK)
	{
		file = SPIFFS.open("/wifiSetup.txt", "r");
		if (file)
		{
			wifiName = file.readStringUntil('\n');
			printf("wifi name: %s\n", wifiName.c_str());
			wifiPass = file.readStringUntil('\n');
			printf("wifi pass: %s\n", wifiPass.c_str());
			file.close();
		}
	}

	/*if (wifiName != "")
	{
		connectToWiFiSTA(wifiName.c_str(), wifiPass.c_str());
	}*/

	while (1)
	{
		numOfClientsConnected = WiFi.softAPgetStationNum();
		// If WiFi is not connected and we have a user connected to our AP, trying to connect keeps changing the WiFi channel and will cause disconnectation of the user from our AP
		if ((numOfClientsConnected > 0) && (WiFi.status() != WL_CONNECTED))
		{
			WiFi.disconnect();
			delay(1000);
			wifiBeginCalled = false;
		}
		if ((numOfClientsConnected == 0) && (WiFi.status() != WL_CONNECTED))
		{
			// only once!!
			if (!wifiBeginCalled)
			{
				WiFi.begin(wifiName.c_str(), wifiPass.c_str());
				wifiBeginCalled = true;
			}
		}
		if ((numOfClientsConnected > 0) || (WiFi.status() == WL_CONNECTED))
		{
			server.handleClient();
			if (wifiSetupPageVisited)
			{
				if (server.args() >= 2)
				{ // Arguments were received
					String ssidName = server.arg(0);
					String ssidPassword = server.arg(1);

					Serial.println(server.args());
					Serial.println((const char*)ssidName.c_str());
					Serial.println((const char*)ssidPassword.c_str());
					wifiSetupPageVisited = false;
					if (spiffsOK)
					{
						if (ssidName != wifiName || ssidPassword != wifiPass)
						{
							file = SPIFFS.open("/wifiSetup.txt", "w");
							if (file)
							{
								sprintf(tempStr, "%s\n", ssidName.c_str());
								file.write((const unsigned char*)tempStr, strlen(tempStr));
								sprintf(tempStr, "%s\n", ssidPassword.c_str());
								file.write((const unsigned char*)tempStr, strlen(tempStr));

								file.flush();
								file.close();
								wifiName = ssidName;
								wifiPass = ssidPassword;
								WiFi.disconnect();
								delay(1000);
								WiFi.begin(wifiName.c_str(), wifiPass.c_str());
							}
						}
					}
				}
			}
			if (toggleIOpageVisited)
			{
				toggleIOpageVisited = false;
				if (server.args() > 0)
				{ // Arguments were received
					String ioState = server.arg(0);

					Serial.println(server.args());
					Serial.println((const char*)ioState.c_str());
				}
			}
		}
	}
}

#define WIFI_TIMEOUT	20 // 10 seconds
bool connectToWiFiSTA(const char* name, const char* pass)
{
	unsigned int timeOutCounter = 0;
	WiFi.begin(name, pass);
	while ((WiFi.status() != WL_CONNECTED) && (timeOutCounter < WIFI_TIMEOUT))
	{
		delay(500);
		Serial.print(".");
		timeOutCounter++;
	}
	if (timeOutCounter != WIFI_TIMEOUT)
	{
		printf("\nWiFi connected.\n");
		Serial.println(WiFi.localIP().toString().c_str());
		Serial.println(WiFi.macAddress());
		return true;
	}
	else
	{
		printf("WiFi: cannot connect to: %s\n", name);
		return false;
	}
}

const char mainMenuPage[]PROGMEM = R"=====(
<!DOCTYPE html> 
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>ESP32 WebPage</title>
<style>
html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}
.button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 10px;}
.button-on {background-color: #1abc9c;}
.button-on:active {background-color: #16a085;}
p {font-size: 14px;color: #888;margin-bottom: 10px;}
</style>
</head>
<body>
<meta charset="utf-8">
<html lang="he">
<h1>Intel's Makers Demo Web Page</h1>
<a class="button button-on" href="/wifiSetupSelected">WiFi Setup</a>
<a class="button button-on" href="/ioPageSelected">IO Control</a>
<a class="button button-on" href="/handleTempHumidityPage">Temp</a>
</body>
<href="/">
</html>)=====";

const char setupWiFiHTMLpart1[]PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
<style>
html {font-family: Arial; display: inline-block; text-align: center;}
.button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 10px;}
.boxStyle {  padding: 12px 20px;  margin: 8px 0;  box-sizing: border-box;  border: 2px solid red;  border-radius: 10px; font-size: 20px;text-align: center;}
</style>
</head>
<body>
<form action="/" method="POST">
Access Point Name:<br>
<input type="text" class="boxStyle" name="AccessPoint" value=
)rawliteral";
//""><br>
const char setupWiFiHTMLpart2[]PROGMEM = R"rawliteral(
Password:<br>
<input type="text" class="boxStyle" name="Password" value=
)rawliteral";
//""><br>
const char setupWiFiHTMLpart3[]PROGMEM = R"rawliteral(
<input type="submit" class="button" value="OK">
</form>
</body>
<href="/">
</html>
)rawliteral";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    .button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 10px;}
  </style>
</head>
<body>
<h2>ESP Web Server</h2>
<h4>IO #1</h4>
<label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="1" checked><span class="slider"></span></label>
<h4>IO #2</h4>
<label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="2" checked><span class="slider"></span></label>
<h4>IO #3</h4>
<label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="3" checked><span class="slider"></span></label>
<h4><a class="button button-on" href="/">Home</a></h4>
<script>function toggleCheckbox(element) {
	var request = new XMLHttpRequest();
	var strOn	= "IO " + element.id + " ON";
	var strOff	= "IO " + element.id + " OFF";
	if(element.checked){ request.open("POST", "", true); request.setRequestHeader("Content-type", " ");request.send(strOn);}
	else { request.open("POST", "", true); request.setRequestHeader("Content-type", " ");request.send(strOff);}
	die();
}</script>
</body>
<href="/">
</html>
)rawliteral";

void handle_OnConnect()
{
	toggleIOpageVisited = false;
	server.send(200, "text/html", mainMenuPage);
}

void wifiSetup_page()
{
	wifiSetupPageVisited = true;
	toggleIOpageVisited = false;
	String setupWiFiHTML = setupWiFiHTMLpart1;
	//""><br>
	setupWiFiHTML += "\"";
	setupWiFiHTML += wifiName;
	setupWiFiHTML += "\"><br>";
	setupWiFiHTML += setupWiFiHTMLpart2;
	setupWiFiHTML += "\"";
	setupWiFiHTML += wifiPass;
	setupWiFiHTML += "\"><br>";
	setupWiFiHTML += setupWiFiHTMLpart3;

	server.send(200, "text/html", setupWiFiHTML);
	printf("wifiSetupPageVisited\n");
}

void toggleIO_page()
{
	toggleIOpageVisited = true;
	wifiSetupPageVisited = false;
	server.send(200, "text/html", index_html);
	printf("toggleIOpageVisited\n");
}

void handle_NotFound()
{
	server.send(404, "text/plain", "Not found");
}
