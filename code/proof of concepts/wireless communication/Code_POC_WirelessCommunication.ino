//bibliotheken importeren
#include <WiFi.h>
#include <WebServer.h>

//instellingen netwerk
const char *ssid = "Linefollower_JVDK";   //naam van het netwerk
const char *password = "ESP32_jelle";     //wachtwoord van het netwerk

WebServer server(80);                     //maak de server aan op poort 80
const int ledPin = 2;                     //GPIO 2 ingebouwde blauwe LED

//html-code voor de webpagina
const char HTML_PAGE[] =
"<!DOCTYPE html><html>"
"<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"<style>"
"body { font-family: Arial; text-align: center; margin-top: 50px; background: #0d1b2a; color: white; }"
".btn { padding: 15px 30px; font-size: 20px; border: none; border-radius: 8px; cursor: pointer; text-decoration: none; display: inline-block; margin: 10px; }"
".btn-on { background-color: #28a745; color: white; }"
".btn-off { background-color: #dc3545; color: white; }"
"</style></head>"
"<body>"
"<h1>ESP32 Besturing</h1>"
"<p>LED Status: <span id=\"status\">Onbekend</span></p>"
"<p>"
"<button class=\"btn btn-on\" onclick=\"stuurStatus('on')\">LED AAN</button>"
"<button class=\"btn btn-off\" onclick=\"stuurStatus('off')\">LED UIT</button>"
"</p>"
"<script>"
"function updateUI(tekst) {"
" document.getElementById('status').innerText = tekst;"
"}"
"function stuurStatus(actie) {"
" fetch('/led/' + actie)"
"   .then(res => res.text())"
"   .then(data => updateUI(data));"
"};"
"</script>"
"</body></html>";

void handleRoot(){
  server.send(200, "text/html", HTML_PAGE);
}

void handleLedOn(){
  digitalWrite(ledPin, HIGH);
  String status = digitalRead(ledPin) ? "AAN" : "UIT";
  server.send(200, "text/plain", status);
}

void handleLedOff(){
  digitalWrite(ledPin, LOW);
  String status = digitalRead(ledPin) ? "AAN" : "UIT";
  server.send(200, "text/plain", status);
}

void handleStatus(){
  String status = digitalRead(ledPin) ? "AAN" : "UIT";
  server.send(200, "text/plain", status);
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
  server.on("/led/status", handleStatus);

  //4. Server daadwerkelijk aanzetten
  server.begin();

}

void loop(){
  server.handleClient();
}
