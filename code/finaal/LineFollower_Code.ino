#include "Cyclus.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "webpage.html.h"
#define Baudrate 115200

Cyclus cyclus;

//instellingen netwerk
const char *ssid = "Linefollower_JVDK";
const char *password = "ESP32_jelle";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//status van de robot
volatile State_t currentState = STATE_STOP;
State_t lastState = (State_t)-1;    //zorgt voor automatische update bij opstarten en statuswissel

//pinnen configureren
const int buttonPin = 14;
const int LED_STATE = 27;
const int LED_WIFI = 26;
const int sensor[] = {36, 39, 34, 35, 32, 33};
const int motors[] = {19, 21, 22, 23};
const int motorEEP = 18;

//andere variabele
volatile unsigned long lastTime = 0;
volatile unsigned long LED_Delay = 0;

void IRAM_ATTR ISR_knop(){
    if(millis() - lastTime > 200){
        if (currentState == STATE_RUN || currentState == STATE_MANUAL){
            currentState = STATE_STOP;
        }
        else if (currentState == STATE_STOP){
            currentState = STATE_RUN;
        }
        lastTime = millis();
  }
}

void logDual(String msg){
    Serial.println(msg);
    ws.textAll(msg);
}

void clearWebMonitor(){
    ws.textAll("CLEAR");
}

void logWebOnly(String msg){
    ws.textAll(msg);
}

void sendStateToWeb(State_t s){
    if (s == STATE_STOP) ws.textAll("STATE: stop");
    else if (s == STATE_RUN) ws.textAll("STATE:run");
    else if (s == STATE_MANUAL) ws.textAll("STATE:manual");
}

// Vangt nieuwe browser-verbindingen en ingetypte web-commando's op
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        // Stuur direct de huidige status naar de browser bij het openen van de pagina
        if (currentState == STATE_STOP) client->text("STATE:stop");
        else if (currentState == STATE_RUN) client->text("STATE:run");
        else if (currentState == STATE_MANUAL) client->text("STATE:manual");
        cyclus.broadcastParams();
    } 
    else if (type == WS_EVT_DISCONNECT) {
        cyclus.stopAllMotors();
    }
    else if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            String cmd = "";
            for (size_t i = 0; i < len; i++) {
                cmd += (char)data[i];
            }
            cmd.trim();
            cyclus.processWebCommand(cmd);
        }
    }
}

void setup() {
    Serial.begin(Baudrate);
    cyclus.begin();
    cyclus.linkState(&currentState);
    cyclus.linkSensors(sensor);
    cyclus.linkMotors(motors);
    cyclus.linkMotorEEP(motorEEP);

    //de status op STOP zetten en de cyclus uitzetten
    currentState = STATE_STOP;
    cyclus.setActive(false);

    //pinnen configureren
    pinMode(buttonPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(buttonPin), ISR_knop, RISING);
    pinMode(LED_STATE, OUTPUT);
    pinMode(LED_WIFI, OUTPUT);
    for (int i = 0; i <4; i++){
        pinMode(motors[i], OUTPUT);
        digitalWrite(motors[i], LOW);
    }
    pinMode(motorEEP, OUTPUT);
    digitalWrite(motorEEP, LOW);

    //verbind met Wi-Fi
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    //WebSocket en Webserver koppelen
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });
    server.begin();

    Serial.println("ready");
}

void loop() {
    ws.cleanupClients();

    //automatische linefollow cyclus actief maken
    cyclus.setActive(currentState == STATE_RUN);
    cyclus.update();

    //detecteert elke statusverandering (hardware en softwarematig)
    if (currentState != lastState){
        lastState = currentState;
        cyclus.stopAllMotors();
        sendStateToWeb(currentState);

        //bij wijziging van status, naar beide monitors printen
        for (int i = 0; i < 50; i++) Serial.println();
        clearWebMonitor();
        if (currentState == STATE_STOP) logDual("state updated to: stop");
        else if (currentState == STATE_RUN) {
            cyclus.resetPID();
            logDual("state updated to: run");
        }
        else if (currentState == STATE_MANUAL) logDual("state updated to: manual");
    }
        
    //de status weergeven op een LED
    switch(currentState){
    case STATE_STOP:
        digitalWrite(LED_STATE, LOW);
        break;
    case STATE_RUN:
        digitalWrite(LED_STATE, HIGH);
        break;
    case STATE_MANUAL:
        if (millis() - LED_Delay >500){
            digitalWrite(LED_STATE, !digitalRead(LED_STATE));
            LED_Delay = millis();
        }
        break;
    
    default:
        break;
    }

    //de wifi-verbinding weergeven op een LED
    digitalWrite(LED_WIFI, WiFi.softAPgetStationNum() > 0 ? HIGH : LOW);
}
