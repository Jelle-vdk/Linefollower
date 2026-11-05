const int ledPin = 27;
const int buttonPin = 14;

volatile bool running = false;
volatile unsigned long lastTime = 0;

void IRAM_ATTR ISR_knop(){
  if(millis() - lastTime > 200){
    running = !running;
    lastTime = millis();
  }
}

void setup(){
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(buttonPin), ISR_knop, RISING);
}

void loop(){
  if (running){
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(500);
  } else {
    digitalWrite(ledPin, LOW);
  }
}
