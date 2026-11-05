const int IN1 = 19;     //motor links vooruit
const int IN2 = 21;     //motor links achteruit
const int IN3 = 22;     //motor rechts vooruit
const int IN4 = 23;     //motor rechts achteruit

const int EEP = 18;     //sleep modus aan/uit


int versnellingen[] = {1, 2, 3, 4, 3, 2, 1};

void setup(){
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(EEP, OUTPUT);

    Serial.begin(9600);
}

void loop(){
  Serial.println("Begin Cyclus ----------------------");
  digitalWrite(EEP, HIGH);

  Serial.println("Motor links vooruit");
  analogWrite(IN1, 200);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
  delay(1000);
  Motoren_Stoppen();

  Serial.println("Motor links achteruit");
  analogWrite(IN1, 0);
  analogWrite(IN2, 200);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
  delay(1000);
  Motoren_Stoppen();

  Serial.println("Motor rechts vooruit");
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 200);
  analogWrite(IN4, 0);
  delay(1000);
  Motoren_Stoppen();

  Serial.println("Motor rechts achteruit");
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 200);
  delay(1000);
  Motoren_Stoppen();

  Serial.println("Motor links versnellen/vertragen");
  for (int i: versnellingen){
    analogWrite(IN1, i*50);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
    delay(1000);
  }
  Motoren_Stoppen();

  Serial.println("Motor rechts versnellen/vertragen");
  for (int i: versnellingen){
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, i*50);
    analogWrite(IN4, 0);
    delay(1000);
  }
  Motoren_Stoppen();

  Serial.println("Beide motoren vooruit");
  analogWrite(IN1, 200);
  analogWrite(IN2, 0);
  analogWrite(IN3, 200);
  analogWrite(IN4, 0);
  delay(1000);
  Motoren_Stoppen();

  Serial.println("Beide motoren achteruit");
  analogWrite(IN1, 0);
  analogWrite(IN2, 200);
  analogWrite(IN3, 0);
  analogWrite(IN4, 200);
  delay(1000);
  Motoren_Stoppen();

  Serial.println("Cycluseinde - 10s rust");
  delay(10000);
}


void Motoren_Stoppen(){
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
  delay(1000);
}
