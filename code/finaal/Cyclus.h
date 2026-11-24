#ifndef CYCLUS_H
#define CYCLUS_H

#include <Arduino.h>
#include "SerialCommand.h"
#include "Memory.h"

struct param_t{
    unsigned long cycleTime;
    int black[6];
    int white[6];
    float Kp;                 //proportionele waarde
    float Ki;                 //integrerende waarde
    float Kd;                 //differentierende waarde
    float power;              //waarde tussen 0 en 255 dus de gerichte snelheid
    float diff;               //waarde voor vermogensverhouding tussen wielen
    //andere parameters dat in het EEPROM moeten opgeslagen worden
};

enum State_t{
    STATE_STOP,
    STATE_RUN,
    STATE_MANUAL
};

void logDual(String msg);
void clearWebMonitor();
void logWebOnly(String msg);

//zorgt dat zowel USB Serial als web-commando's doorgeeft aan SerialCommand
class DualInputStream : public Stream {
  private:
    String webBuffer = "";
  public:
    void feedWebCommand(String cmd) {
        webBuffer += cmd + "\r";
    }
    int available() override {
        return Serial.available() + webBuffer.length();
    }
    int read() override {
        if (Serial.available()) return Serial.read();
        if (webBuffer.length() > 0) {
            char c = webBuffer[0];
            webBuffer.remove(0, 1);
            return c;
        }
        return -1;
    }
    int peek() override {
        if (Serial.available()) return Serial.peek();
        if (webBuffer.length() > 0) return webBuffer[0];
        return -1;
    }
    size_t write(uint8_t b) override { return Serial.write(b); }
};

class Cyclus {
  private:
    static DualInputStream dualStream;
    static SerialCommand sCmd;

    //alle instelbare variabelen
    static bool debug;                                              //debug aan en uit zetten
    static bool running;                                            //de cyclus aan en uit zetten
    static unsigned long previous, calculationTime;                 //de vorige en rekentijd bijhouden voor in de cyclus
    static param_t params;                                          //houd alle opgeslagen parameters bij
    static volatile State_t* statePtr;                              //pointer naar currentState van de code.ino
    static const int* motorPins;                                    //pointer naar alle motorpinnen
    static const int* sensorPins;                                   //pointer naar alle sensorpinnen
    static int motorEEPPin;                                         //pointer naar de EEP pin van de H-brug
    static long normalised[6];
    static float position;
    static float error;
    static float iTerm;
    static float lastErr;
    static float output;
    static float powerLeft;
    static float powerRight;
    static int offLineCount;

    // Statische opdrachten voor SerialCommand
    static void onSet() {
      char* param = sCmd.next();
      char* value = sCmd.next();

        //als het commando incompleet is
        if (param == NULL || value == NULL){
            clearMonitor();
            logDual("error: missing parameter or value (usage: set <param> <val>)");
            return;
        }

        //commando set cycle ...
        else if (strcmp(param, "cycle") == 0){
            long newCycleTime = atol(value);
            if (params.cycleTime > 0 && newCycleTime > 0){
                float ratio = ((float)newCycleTime) / ((float)params.cycleTime);
                params.Ki *= ratio;
                params.Kd /= ratio;
            }
            params.cycleTime = newCycleTime;
            EEPROM_writeAnything(0, params);
            clearMonitor();
            logDual("cycle time updated to: " + String(params.cycleTime));
        }
        //commando set state ...
        else if (strcmp(param, "state") == 0){
            setMotors(0, 0);
            if (statePtr == nullptr) return;
            else if (strcmp(value, "run") == 0) *statePtr = STATE_RUN;
            else if (strcmp(value, "stop") == 0) *statePtr = STATE_STOP;
            else if (strcmp(value, "manual") == 0) *statePtr = STATE_MANUAL;
            else{
                clearMonitor();
                logDual("error: unknown state (allowed: run, stop, manual)");
                return;
            }
        }
        //commando set power ...
        else if(strcmp(param, "power") == 0){
            params.power = atof(value);
            EEPROM_writeAnything(0, params);
            clearMonitor();
            logDual("power updated to: " + String(params.power));
        }
        //commando set diff ...
        else if(strcmp(param, "diff") == 0){
            params.diff = atof(value);
            EEPROM_writeAnything(0, params);
            clearMonitor();
            logDual("diff updated to: " + String(params.diff));
        }
        //commando set Kp ...
        else if(strcmp(param, "Kp") == 0 || strcmp(param, "kp") == 0){
            params.Kp = atof(value);
            EEPROM_writeAnything(0, params);
            clearMonitor();
            logDual("Kp updated to: " + String(params.Kp));
        }
        //commando set Ki ...
        else if(strcmp(param, "Ki") == 0 || strcmp(param, "ki") == 0){
            float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
            params.Ki = atof(value)*cycleTimeInSec;
            EEPROM_writeAnything(0, params);
            clearMonitor();
            logDual("Ki updated to: " + String(value));
        }
        //commando set Kd ...
        else if(strcmp(param, "Kd") == 0 || strcmp(param, "kd") == 0){
            float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
            params.Kd = atof(value)/cycleTimeInSec;
            EEPROM_writeAnything(0, params);
            clearMonitor();
            logDual("Kd updated to: " + String(value));
        }

        //als de parameter niet beschikbaar is
        else{
            clearMonitor();
            logDual("unknown parameter: \"" + String(param) + "\"");
        }

        sendParamsToWeb();
    }

    static void onCalibrate(){
        char* param = sCmd.next();

        if(sensorPins == nullptr){
            clearMonitor();
            logDual("error: sensor pins not linked");
            return;
        }

        if(param == NULL){
            clearMonitor();
            logDual("error: missing parameter (usage: calibrate black or white)");
            return;
        }
        else if (strcmp(param, "black") == 0){
            clearMonitor();
            logDual("start calibrating black ...");
            for (int i = 0; i<6; i++){
                long som = 0;
                for (int j = 0; j<10; j++){
                    som += analogRead(sensorPins[i]);
                }
                params.black[i] = som/10;
            }
            EEPROM_writeAnything(0, params);
            logDual("done calibrating black!");
        }
        else if (strcmp(param, "white") == 0){
            clearMonitor();
            logDual("start calibrating white ...");
            for (int i = 0; i<6; i++){
                long som = 0;
                for (int j = 0; j<10; j++){
                    som += analogRead(sensorPins[i]);
                }
                params.white[i] = som/10;
            }
            EEPROM_writeAnything(0, params);
            logDual("done calibrating white!");
        }
        else{
            clearMonitor();
            logDual("unknown parameter: \"" + String(param) + "\" (use: black or white)");
        }
    }

    static void onDebug() {
        clearMonitor();

        //de cyclustijd
        logDual("cycle time: " + String(params.cycleTime));

        //PID en motor parameters
        logDual("power: " + String(params.power));
        logDual("diff: " + String(params.diff, 2));
        logDual("Kp: " + String(params.Kp, 2));
        float cycleTimeInSec = ((float)params.cycleTime) / 1000000;
        float kiSec = (cycleTimeInSec > 0) ? (params.Ki / cycleTimeInSec): 0.0;
        logDual("Ki: " + String(kiSec, 2));
        float kdSec = params.Kd * cycleTimeInSec;
        logDual("Kd: " + String(kdSec, 2));

        //zwarte en witte gekalibreerde waarden
        String blackStr = "black: ";
        String whiteStr = "white: ";
        for (int i = 0; i<6; i++){
            blackStr += String(params.black[i]) + " ";
            whiteStr += String(params.white[i]) + " ";
        }
        logDual(blackStr);
        logDual(whiteStr);

        //rekentijd
        logDual("calculation time: " + String(calculationTime));
        calculationTime = 0;

        sendParamsToWeb();
    }

    static void onUnknownCommand(char *command) {
        clearMonitor();
        logDual("unknown command: \"" + String(command) + "\"");
    }

    static void clearMonitor(){
        for (int i = 0; i<50; i++) Serial.println();
        clearWebMonitor();
    }

    static void setMotorPWM(int in1, int in2, int speed){
        speed = constrain (speed, -255, 255);
        if (speed > 0){
            analogWrite(in1, speed);
            analogWrite(in2, 0);
        } else if (speed < 0){
            analogWrite(in1, 0);
            analogWrite(in2, -speed);
        } else{
            analogWrite(in1, 0);
            analogWrite(in2, 0);
        }
    }

    static void setMotors(int leftSpeed, int rightSpeed){
        if (motorPins == nullptr) return;
        if (motorEEPPin != -1){
            digitalWrite(motorEEPPin, (leftSpeed != 0 || rightSpeed != 0) ? HIGH : LOW);
        }
        setMotorPWM(motorPins[0], motorPins[1], leftSpeed);
        setMotorPWM(motorPins[2], motorPins[3], rightSpeed);
    }

    static void onDrive(){
        if (statePtr == nullptr || *statePtr != STATE_MANUAL){
            setMotors(0, 0);
            return;
        }

        char* dir = sCmd.next();
        char* speedStr = sCmd.next();

        if (dir == NULL || strcmp(dir, "stop") == 0){
            setMotors(0, 0);
            return;
        }

        int spd = (speedStr != NULL) ? constrain(atoi(speedStr), 0, 255) : 0;

        if (strcmp(dir, "forward") == 0) setMotors(spd, spd);
        else if (strcmp(dir, "backward") == 0) setMotors(-spd, -spd);
        else if (strcmp(dir, "left") == 0) setMotors(-spd, spd);
        else if (strcmp(dir, "right") == 0) setMotors(spd, -spd);
        else setMotors(0, 0);
    }

    static void normalisatie_interpolatie(){
        if (sensorPins == nullptr) return;

        //normaliseren sensor
        for (int i = 0; i<6; i++){
            long value = analogRead(sensorPins[i]);
            normalised[i] = map(value, params.white[i], params.black[i], 0, 1000);
            normalised[i] = constrain(normalised[i], 0, 1000);
        }
        //interpoleren sensor
        int index = 0;
        for (int i = 1; i<6; i++){
            if (normalised[i] > normalised[index]) index = i;
        }

        if (index == 0) index = 1;
        else if (index == 5) index = 4;

        long sZero = normalised[index];
        long sMinusOne = normalised[index - 1];
        long sPlusOne = normalised[index + 1];

        float b = ((float)(sPlusOne - sMinusOne)) / 2.0;
        float a = sPlusOne - b - sZero;

        position = (a != 0) ? (-b / (2.0 * a)) : 0.0;
        position += index - 2.5;
        position *= 9.525;
    }

    static void sendParamsToWeb(){
        float cycleTimeInSec = ((float)params.cycleTime) / 1000000.0;
        float kiSec = (cycleTimeInSec > 0) ? (params.Ki / cycleTimeInSec) : 0.0;
        float kdSec = params.Kd * cycleTimeInSec;
        String msg = "PARAMS:" + String(params.power, 1) + "|" + String(params.diff, 2) + "|" + String(params.Kp, 2) + "|" + String(kiSec, 2) + "|" + String(kdSec, 2);
        logWebOnly(msg);
    }

  public:
    void processWebCommand(String cmd){
        dualStream.feedWebCommand(cmd);
    }
    void broadcastParams(){
        sendParamsToWeb();
    }

    void linkState(volatile State_t* extState){
        statePtr = extState;
    }
    void linkSensors(const int* extSensors){
        sensorPins = extSensors;
    }
    void linkMotors(const int* extMotors){
        motorPins = extMotors;
    }
    void linkMotorEEP(int extEEP){
        motorEEPPin = extEEP;
    }

    void stopAllMotors(){
        setMotors(0, 0);
    }

    void resetPID(){
        iTerm = 0;
        lastErr = 0;
        offLineCount = 0;
    }

    void begin() {
        //alle variabele direct juist instellen
        debug = false;
        running = false;
        previous = micros();
        

        //EEPROM starten en direct data ophalen
        EEPROM.begin(512);
        EEPROM_readAnything(0, params);
        if (params.cycleTime == 0 || params.cycleTime == 0xFFFFFFFF) params.cycleTime = 1000000;  //voor als het geheugen leeg/ongeldig was

        // Commando's registreren in de library zelf
        sCmd.addCommand("set", onSet);
        sCmd.addCommand("debug", onDebug);
        sCmd.addCommand("calibrate", onCalibrate);
        sCmd.addCommand("drive", onDrive);
        sCmd.setDefaultHandler(onUnknownCommand);
    }

    void setActive(bool state) {
        running = state;
    }

    //cyclus zelf dat elke keer uitgevoerd word
    void update() {
        // Luistert continue naar de seriele commando's
        sCmd.readSerial();

        // 1. In MANUAL: elke 40ms meten + naar live webgrafiek sturen
        if (statePtr != nullptr && *statePtr == STATE_MANUAL){
            static unsigned long lastWebSensorUpdate = 0;
            if (millis() - lastWebSensorUpdate >= 40){
                lastWebSensorUpdate = millis();
                normalisatie_interpolatie();

                String msg = "SENSORS:";
                for (int i = 0; i < 6; i++){
                    msg += String(normalised[i]) + (i < 5 ? "," : "");
                }
                msg += "|" + String(position, 2);
                logWebOnly(msg);
            }
            return;
        }

        //2. In RUN: op maximale snelheid meten + PID + motoren
        if (!running) return;

        unsigned long current = micros();
        if (current - previous >= params.cycleTime) {
            previous = current;

            //hier komt de code dat cyclisch uitgevoert moet worden

            //normalisatie_interpolatie
            normalisatie_interpolatie();

            //veiligheid: stop als alle sensoren wit zien (lijn kwijt of opgetild)
            bool allWhite = true;
            for (int i = 0; i< 6; i++){
                if (normalised[i] > 200){
                    allWhite = false;
                    break;
                }
            }

            if (allWhite){
                offLineCount++;
                if (offLineCount >= 10){
                    stopAllMotors();
                    if (statePtr != nullptr) *statePtr = STATE_STOP;
                    return;
                }
            } else {
                offLineCount = 0;
            }

            //PID regeling
            error = position;

            //P
            output = error*params.Kp;
            //I
            iTerm += params.Ki * error;
            iTerm = constrain(iTerm, -510, 510);
            output += iTerm;
            //D
            output += params.Kd * (error-lastErr);
            lastErr = error;
            
            //finale output
            output = constrain(output, -510, 510);

            //aansturen motoren
            if (output>=0){
                powerLeft = constrain(params.power + params.diff*output, -255, 255);
                powerRight = constrain(powerLeft - output, -255, 255);
                powerLeft = powerRight + output;
            }
            else{
                powerRight = constrain(params.power - params.diff*output, -255, 255);
                powerLeft = constrain(powerRight + output, -255, 255);
                powerRight = powerLeft - output;
            }

            setMotors(powerLeft, powerRight);
        }

        unsigned long difference = micros() - current;
        if (difference > calculationTime) calculationTime = difference;
    }
};

// Statische variabelen declareren
DualInputStream Cyclus::dualStream;
SerialCommand Cyclus::sCmd(Cyclus::dualStream);
bool Cyclus::debug = false;
bool Cyclus::running = false;
unsigned long Cyclus::previous = 0;
unsigned long Cyclus::calculationTime = 0;
param_t Cyclus::params;
volatile State_t* Cyclus::statePtr = nullptr;
const int* Cyclus::sensorPins = nullptr;
const int* Cyclus::motorPins = nullptr;
int Cyclus::motorEEPPin = -1;
long Cyclus::normalised[6] = {0};
float Cyclus::position = 0.0;
float Cyclus::error = 0;
float Cyclus::iTerm = 0;
float Cyclus::lastErr = 0;
float Cyclus::output = 0;
float Cyclus::powerLeft = 0;
float Cyclus::powerRight = 0;
int Cyclus::offLineCount = 0;
#endif