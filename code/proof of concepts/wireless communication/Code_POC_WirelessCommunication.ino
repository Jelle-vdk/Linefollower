//bibliotheken importeren
#include <WiFi.h>
#include <WebServer.h>

//instellingen netwerk
const char *ssid = "ESP32_Robot_Jelle";   //naam van het netwerk
const char *password = "ESP32_jelle";     //wachtwoord van het netwerk

WebServer server(80);                     //maak de server aan op poort 80
const int ledPin = 2;                     //GPIO 2 ingebouwde blauwe LED

//html-code voor de webpagina
const char HTML_PAGE[] =
"<!DOCTYPE html><html>"
"<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"<style>"
"body { font-family: Arial; text-align: center; margin-top: 50px; background: #220; color: white; }"
".btn { padding: 15px 30px; font-size: 20xp; border: none; border-radius: 8xp; cursor: pointer; text-decoration: none; display: inline-block; margin: 10px; }"
".btn-on { background-color: #28a745; color: white; }"
".btn-off { background-color: #dc3545; color: white; }"
"</style></head>"
"<body>"
"<h1>ESP32 Besturing</h1>"
"<p><a href=\"/led/on\"><button class=\"btn btn-on\">LED AAN</button></a></p>"
"<p><a href=\"/led/off\"><button class=\"btn btn-off\">LED UIT</button></a></p>"
"</body></html>";

void handleRoot(){
  server.send(200, "text/html", HTML_PAGE);
}

void handleLedOn(){
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLedOff(){
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup(){
  Serial.begin(115200);

  //1. Hardware instellen
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  //2. Wi-Fi Acces Point starten
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();

  Serial.println("Acces Point gestart!");
  Serial.print("Surf naar: http://");
  Serial.println(IP);

  //3. Routing: koppel URL's aan de juiste acties
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);

  //4. Server daadwerkelijk aanzetten
  server.begin();

}

void loop(){
  server.handleClient();
}
