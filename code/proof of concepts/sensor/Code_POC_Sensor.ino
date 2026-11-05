int sensorList[] = {36, 39, 34, 35, 32, 33};

void setup(){
  for (int x: sensorList){
    pinMode(x, INPUT);
  }

  Serial.begin(9600);
}

void loop(){
  String tekstData = "Uitgelezen data:";

  for (int x: sensorList){
    int waarde = analogRead(x);
    tekstData += " " + String(waarde);
  }

  Serial.println(tekstData);

  delay(1000);
}
