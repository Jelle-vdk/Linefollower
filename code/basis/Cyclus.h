#ifndef CYCLUS_H
#define CYCLUS_H

#include <Arduino.h>
#include "SerialCommand.h"
#include "Memory.h"

struct param_t{
    unsigned long cycleTime;
    //andere parameters dat in het EEPROM moeten opgeslagen worden
};

enum State_t{
    STATE_STOP,
    STATE_RUN,
    STATE_MANUAL
};

class Cyclus {
  private:
    static SerialCommand sCmd;

    //alle instelbare variabelen
    static bool debug;                                              //debug aan en uit zetten
    static bool running;                                            //de cyclus aan en uit zetten
    static unsigned long previous, calculationTime;                 //de vorige en rekentijd bijhouden voor in de cyclus
    static param_t params;                                          //houd alle opgeslagen parameters bij
    static volatile State_t* statePtr;                              //pointer naar currentState van de code.ino

    // Statische opdrachten voor SerialCommand
    static void onSet() {
      char* param = sCmd.next();
      char* value = sCmd.next();

        //als het commando incompleet is
        if (param == NULL || value == NULL){
            clearMonitor();
            Serial.println("error: missing parameter or value (usage: set <param> <val>)");
            return;
        }

        //commando set cycle ...
        if (strcmp(param, "cycle") == 0){
            params.cycleTime = atol(value);
            EEPROM_writeAnything(0, params);
            clearMonitor();
            Serial.print("cycle time updated to: ");
            Serial.println(params.cycleTime);
        }
        //commando set state ...
        if (strcmp(param, "state") == 0){
            if (strcmp(value, "run") == 0) *statePtr = STATE_RUN;
            else if (strcmp(value, "stop") == 0) *statePtr = STATE_STOP;
            else if (strcmp(value, "manual") == 0) *statePtr = STATE_MANUAL;
            else{
                clearMonitor();
                Serial.println("error: unknown state (allowed: run, stop, manual)");
                return;
            }
            clearMonitor();
            Serial.print("state updated to: ");
            Serial.println(value);
        }

        //als de parameter niet beschikbaar is
        else{
            clearMonitor();
            Serial.print("unknown parameter: \"");
            Serial.print(param);
            Serial.print("\"");
        }
    }

    static void onDebug() {
        clearMonitor();
        Serial.print("cycle time: ");
        Serial.println(params.cycleTime);

        //hier komen de parameters dat we willen zien na het debug commando

        Serial.print("calculation time: ");
        Serial.println(calculationTime);
        calculationTime = 0;
    }

    static void onUnknownCommand(char *command) {
        clearMonitor();
        Serial.print("unknown command: \"");
        Serial.print(command);
        Serial.println("\"");
    }

    static void clearMonitor(){
        for (int i = 0; i<50; i++) Serial.println();
    }

  public:
    void linkState(volatile State_t* extState){
        statePtr = extState;
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
        sCmd.setDefaultHandler(onUnknownCommand);
    }

    void setActive(bool state) {
        running = state;
    }

    //cyclus zelf dat elke keer uitgevoerd word
    void update() {
        // Luistert continue naar de seriele commando's
        sCmd.readSerial();

        // Timer en acties draaien enkel als de cyclus actief is
        if (!running) return;

            unsigned long current = micros();
            if (current - previous >= params.cycleTime) {
                previous = current;

                //hier komt de code dat cyclisch uitgevoert moet worden

                //normaliseren en interpoleren sensor
                //PID regeling
                //aansturen motoren
            }

            unsigned long difference = micros() - current;
            if (difference > calculationTime) calculationTime = difference;
        }
};

// Statische variabelen declareren
SerialCommand Cyclus::sCmd(Serial);
bool Cyclus::debug = false;
bool Cyclus::running = false;
unsigned long Cyclus::previous = 0;
unsigned long Cyclus::calculationTime = 0;
param_t Cyclus::params;
volatile State_t* Cyclus::statePtr = nullptr;

#endif