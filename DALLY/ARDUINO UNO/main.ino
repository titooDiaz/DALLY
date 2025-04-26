// RGB LED Pins
const int redPin   = 9;
const int greenPin = 10;
const int bluePin  = 11;

// Water Pump
const int pumpPin  = 7;

// Potentiometer
const int potPin   = A1;
int potStatus      = 0;    // desired level (1–5)
int lastPotValue   = 0;

// Real humidity sensor
const int moisturePin = A0;
int sensorStatus     = 0;  // current level (1–5)

// Buzzer
const int buzzerPin = 8;
bool farewellPlayed = false;

// Timings
unsigned long lastPotMove      = 0;
const unsigned long potDelay   = 2000;   // 2 s without movement
unsigned long lastBuzzer       = 0;
const unsigned long buzzerIntrv = 10000; // 10 s

// Pump state
bool pumpOn = false;

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  int init = analogRead(moisturePin);
  sensorStatus = readHumidityLevel(init);
  showHumidity(init);
}

void loop() {
  unsigned long now = millis();

  // Read potentiometer and detect movement
  int potValue = analogRead(potPin);
  if (abs(potValue - lastPotValue) > 5) {
    lastPotMove = now;
  }
  lastPotValue = potValue;

  // Always read sensor
  int realValue     = analogRead(moisturePin);
  sensorStatus      = readHumidityLevel(realValue);
  unsigned long sincePot = now - lastPotMove;

  // Choose what to display on the LED
  if (sincePot <= potDelay) {
    // Potentiometer mode
    potStatus = readHumidityLevel(potValue); // ✅ BEFORE, this was only updated if moved!
    showHumidity(potValue);
    Serial.print("POT ("); Serial.print(potValue);
    Serial.print(") → desired: "); Serial.println(potStatus);
  } else {
    // Sensor mode
    showHumidity(realValue);
    Serial.print("SENSOR ("); Serial.print(realValue);
    Serial.print(") → current: "); Serial.println(sensorStatus);

    if (now - lastBuzzer >= buzzerIntrv) {
      lastBuzzer = now;
    }
  }

  // Pump control
  if (sensorStatus > potStatus) {
    if (!pumpOn) {
      digitalWrite(pumpPin, HIGH);
      pumpOn = true;
      farewellPlayed = false;
      Serial.println(">>> PUMP TURNED ON");
    }
  } else {
    if (pumpOn) {
      digitalWrite(pumpPin, LOW);
      pumpOn = false;
      Serial.println(">>> PUMP TURNED OFF");
      if (!farewellPlayed) {
        playFarewell();
        farewellPlayed = true;
      }
    }
  }
}

// Corrected levels to avoid duplicate level 1
int readHumidityLevel(int v) {
  if (v <= 300)      return 1;
  else if (v <= 450) return 2;  // ✅ before this also returned 1
  else if (v <= 600) return 3;
  else if (v <= 750) return 4;
  else               return 5;
}

// Without annoying delay
void showHumidity(int v) {
  int r, g, b;
  if (v <= 300)      { r=100; g=200; b=150; }
  else if (v <= 450) { r=0;   g=0;   b=255; }
  else if (v <= 600) { r=0;   g=255; b=0;   }
  else if (v <= 750) { r=255; g=255; b=0;   }
  else               { r=255; g=0;   b=0;   }

  analogWrite(redPin,   r);
  analogWrite(greenPin, g);
  analogWrite(bluePin,  b);

  // ✅ delay(10) removed: not necessary, could freeze loop
}

// Melody
void playFarewell() {
  tone(buzzerPin, 660); delay(400);
  tone(buzzerPin, 440); delay(400);
  tone(buzzerPin, 330); delay(500);
  noTone(buzzerPin);
}
