/*

Simple HullPixelbot web server

Based on:

ESP8266 mDNS responder sample

This is an example of an HTTP server that is accessible
via http://hullpixelbot.local URL thanks to mDNS responder.

Instructions:
- Update WiFi SSID and password as necessary.
- Flash the sketch to the ESP8266 board
- Install host software:
- For Linux, install Avahi (http://avahi.org/).
- For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
- For Mac OSX and iOS support is built in through Bonjour already.
- Point your browser to http://hullpixelbot.local, you should see a response.

- Add a HullPixelbot query to send a message out of the serial port to the robot motor unit

// http://hullpixelbot.local?HullPixelbot=MF100

// This would cause the robot to move forwards 100 steps
// Details of available robot commands can be found here:

// https://github.com/HullPixelbot/HullPixelbotCode

// Version 1.0 Rob Miles February 2017

*/

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "ssid";
const char *password = "password";

ESP8266WebServer server(80);

const int led = 13;

char hostString[30] = { 0 };

int version = 1;

// These are placeholder values
// If you get sensor values back from the robot you can put them in these
// variables and they will be sent back to the web client as a json response

int distanceReading = 1;
int leftLightSensor = 2;
int rightLightSensor = 3;
int centreLightSensor = 4;

int leftMotorSpeed = 6;
int rightMotorSpeed = 7;

void sendRobotCommand(String command)
{
	Serial.print((char)0x0d);
	Serial.print(command);
	Serial.print((char)0x0d);
}

void handleRoot() {
	digitalWrite(led, 1);

	char temp[400];

	String robotcommand = server.arg("hullpixelbot");

	if (robotcommand.length() > 0)
	{
		sendRobotCommand(robotcommand);
	}

	// Construct some JSON to send back to the host. This can contain values (although at the moment it doesn't)

	snprintf(temp, 400,
		"{\"version\":%d,\"distance\":[%d],\"lightLevel\":[%d,%d,%d],\"leftSpeed\":%d,\"rightSpeed\":-%d}",
		version, distanceReading, leftLightSensor, rightLightSensor, centreLightSensor, leftMotorSpeed, rightMotorSpeed);
	server.send(200, "text/json", temp);
	digitalWrite(led, 0);
}

void handleNotFound() {
	digitalWrite(led, 1);
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}

	server.send(404, "text/plain", message);
	digitalWrite(led, 0);
}

void setup(void)
{
	Serial.begin(1200);

	// Connect to WiFi network
	WiFi.begin(ssid, password);

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
		sendRobotCommand("PC0,0,255");
		delay(500);
		sendRobotCommand("PC255,0,0");
		delay(500);
	}

	// This sets the network name
	// If you have more than one HullPixelbot you will have to
	// change this. 

	sprintf(hostString, "HullPixelbot", ESP.getChipId());

	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.print("Hostname: ");
	Serial.println(hostString);

	// Set up mDNS responder:
	// - first argument is the domain name, in this example
	//   the fully-qualified domain name is "esp8266.local"
	// - second argument is the IP address to advertise
	//   we send our IP address on the WiFi network

	if (!MDNS.begin(hostString)) {

		Serial.println("Error setting up MDNS responder!");

		// Light up white 
		sendRobotCommand("PC255,255,255");

		while (1) {
			delay(1000);
		}
	}

	Serial.println("mDNS responder started");

	// Start TCP (HTTP) server
	server.on("/", handleRoot);

	server.onNotFound(handleNotFound);

	server.begin();

	Serial.println("HTTP server started");

	// Add service to MDNS-SD
	MDNS.addService("http", "tcp", 80);

	// Light up green to indicate all is well. 

	sendRobotCommand("PC0,255,0");
}

void loop(void)
{
	server.handleClient();

	MDNS.update();

	return;

}

