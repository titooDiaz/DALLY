int humiditySensor = A0;
const int pumpPin = 8;
const int potPin = A1;
const int buzzerPin = 7;
boolean pupState = false;
const int rainPin = 12;

// RGB leds
const int R = 9;
const int G = 10;
const int B = 11;

//potState
int potState;
int lastPotValue;

// step motor
int in1 = 6;
int in2 = 5;
int in3 = 4;
int in4 = 3;

unsigned long lastStepTime = 0;
const unsigned long stepInterval = 1400; // us
int stepIndex = 0;

unsigned long lastMainTime = 0;
const unsigned long mainInterval = 200;

// half-step (8 steps)
int steps[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

void setup() {
  Serial.begin(9600);
  //rainSensor
  pinMode(rainPin, INPUT);
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

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop() {
  runStepper();

  unsigned long now = millis();
  if (now - lastMainTime >= mainInterval) {
    lastMainTime = now;

    int humidity = analogRead(humiditySensor)/205;
    int potState = analogRead(potPin)/205;

    // water pump logic
    if (humidity < potState && !pupState){
      pupState = true;
      digitalWrite(pumpPin, HIGH);
      pupOn();
    } else if (pupState && humidity >= potState){
      pupState = false;
      digitalWrite(pumpPin, LOW);
      pupOff();
    }

    //potState logic
    if (potChange(lastPotValue, analogRead(potPin))){
      setColor(potState);
    } else {
      setColor(humidity);
    }

    lastPotValue = analogRead(potPin);

    int rainState = digitalRead(rainPin);
    Serial.println(rainState);
  }
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

void runStepper() {
  unsigned long now = micros();

  if (now - lastStepTime >= stepInterval) {
    lastStepTime = now;

    digitalWrite(in1, steps[stepIndex][0]);
    digitalWrite(in2, steps[stepIndex][1]);
    digitalWrite(in3, steps[stepIndex][2]);
    digitalWrite(in4, steps[stepIndex][3]);

    stepIndex++;
    if (stepIndex >= 8) stepIndex = 0;
  }
}
