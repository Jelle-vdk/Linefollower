#include "cyclus.h"
#define Baudrate 115200

Cyclus cyclus;

//status van de robot
volatile State_t currentState = STATE_STOP;

//pinnen configureren
const int buttonPin = 14;
const int LED_STATE = 27;

//andere variabele
volatile unsigned long lastTime = 0;
volatile unsigned long LED_Delay = 0;

void IRAM_ATTR ISR_knop(){
  if(millis() - lastTime > 200){
    if (currentState == STATE_RUN){
      currentState = STATE_STOP;
      clearMonitor();
      Serial.println("state updated to: stop");
    }
    else if (currentState == STATE_MANUAL){
      currentState = STATE_STOP;
      clearMonitor();
      Serial.println("state updated to: stop");
    }
    else if (currentState == STATE_STOP){
      currentState = STATE_RUN;
      clearMonitor();
      Serial.println("state updated to: run");
    }
    lastTime = millis();
  }
}

void clearMonitor(){
  for (int i = 0; i<50; i++) Serial.println();
}

void setup() {
  Serial.begin(Baudrate);
  cyclus.begin();
  cyclus.linkState(&currentState);

  //de status op STOP zetten en de cyclus uitzetten
  currentState = STATE_STOP;
  cyclus.setActive(false);

  //pinnen configureren
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), ISR_knop, RISING);
  pinMode(LED_STATE, OUTPUT);

  Serial.println("ready");
}

void loop() {
  //automatische linefollow cyclus actief maken
  cyclus.setActive(currentState == STATE_RUN);
  cyclus.update();

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
}