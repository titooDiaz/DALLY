int humiditySensor = A0;
const int pumpPin = 8;
const int potPin = A1;
const int buzzerPin = 7;
boolean pupState = false;

// RGB leds
const int R = 9;
const int G = 10;
const int B = 11;

//potState
int potState;
int lastPotValue;

void setup() {
  Serial.begin(9600);

  // water pump
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // RGB LED init
  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 255);

  // init pot State
  potState = analogRead(potPin)/5;
}

void loop() {
  int humidity = analogRead(humiditySensor)/205;
  int potState = analogRead(potPin)/205;
  
  Serial.println(potState/205);
  delay(100);

  // water pump logic
  if (humidity<potState and !pupState){
    pupState = true;
    digitalWrite(pumpPin, HIGH);
    pupOn();
  }else if (pupState and humidity >= potState){
    pupState = false;
    digitalWrite(pumpPin, LOW);
    pupOff();
  }

  //potState logic
  if (potChange(lastPotValue, analogRead(potPin))){
    setColor(potState);
    delay(500);
  }else{
    setColor(humidity);
  }


  lastPotValue = analogRead(potPin);
}


// potState logic
void setColor(int state) {
  switch (state) {
  
    case 4: // red
      analogWrite(R, 255);
      analogWrite(G, 0);
      analogWrite(B, 0);
      break;
  
    case 3: // orange
      analogWrite(R, 255);
      analogWrite(G, 120);
      analogWrite(B, 0);
      break;
  
    case 2: // green
      analogWrite(R, 0);
      analogWrite(G, 255);
      analogWrite(B, 0);
      break;
  
    case 1: //blue
      analogWrite(R, 0);
      analogWrite(G, 0);
      analogWrite(B, 255);
      break;
  
    case 0: // white
      analogWrite(R, 255);
      analogWrite(G, 255);
      analogWrite(B, 255);
      break;
  
    default: // Seguridad
      analogWrite(R, 0);
      analogWrite(G, 0);
      analogWrite(B, 0);
      break;
  }
}

boolean potChange(int last, int now){
  if (abs(last-now)>5){
    return true;
  }
  return false;
}

void pupOn() {
  tone(buzzerPin, 40);  // G4
  delay(300);
  noTone(buzzerPin);
}

void pupOff() {
  tone(buzzerPin, 988);  // B5
  delay(150);
  tone(buzzerPin, 1175); // D6
  delay(150);
  noTone(buzzerPin);
}