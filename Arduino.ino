// Pin assignments
const int sensorPins[4] = {A0, A1, A2, A3};
const int greenPins[4] = {3, 6, 9, 12};
const int yellowPins[4] = {4, 7, 10, 13};
const int redPins[4] = {5, 8, 11, A4}; // A4 used for red of lane 4
const int emergencyPin = 2;

// Traffic control timings (ms)
const unsigned long baseGreen = 10000;
const unsigned long maxGreen = 30000;
const unsigned long yellowTime = 3000;

// Sensor & traffic thresholds
const int sensorThreshold = 300;
const int heavyThreshold = 600;

// --- Function Declarations ---
void handleEmergency();
void cycleSignal(int lane, unsigned long greenTime);

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(sensorPins[i], INPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(yellowPins[i], OUTPUT);
    pinMode(redPins[i], OUTPUT);
    digitalWrite(greenPins[i], LOW);
    digitalWrite(yellowPins[i], LOW);
    digitalWrite(redPins[i], HIGH); // Start all red
  }
  pinMode(emergencyPin, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(emergencyPin) == LOW) {
    handleEmergency();
    return;
  }

  int densities[4];
  for (int i = 0; i < 4; i++) {
    densities[i] = analogRead(sensorPins[i]);
    Serial.print("Lane "); Serial.print(i);
    Serial.print(": "); Serial.println(densities[i]);
  }

  int maxLane = 0;
  for (int i = 1; i < 4; i++) {
    if (densities[i] > densities[maxLane]) maxLane = i;
  }

  unsigned long greenTime = baseGreen;
  if (densities[maxLane] > heavyThreshold) {
    greenTime = min(maxGreen, map(densities[maxLane], heavyThreshold, 1023, baseGreen, maxGreen));
  }

  cycleSignal(maxLane, greenTime);
}

void cycleSignal(int lane, unsigned long greenTime) {
  for (int i = 0; i < 4; i++) {
    if (i == lane) {
      digitalWrite(redPins[i], LOW);
      digitalWrite(greenPins[i], HIGH);
    } else {
      digitalWrite(greenPins[i], LOW);
      digitalWrite(yellowPins[i], LOW);
      digitalWrite(redPins[i], HIGH);
    }
  }

  delay(greenTime);
  digitalWrite(greenPins[lane], LOW);
  digitalWrite(yellowPins[lane], HIGH);
  delay(yellowTime);
  digitalWrite(yellowPins[lane], LOW);
  digitalWrite(redPins[lane], HIGH);
}

void handleEmergency() {
  Serial.println("Emergency detected! Clearing intersection...");
  for (int i = 0; i < 4; i++) {
    digitalWrite(greenPins[i], LOW);
    digitalWrite(yellowPins[i], LOW);
    digitalWrite(redPins[i], LOW);
  }

  unsigned long startTime = millis();
  while (digitalRead(emergencyPin) == LOW) {
    if ((millis() - startTime) > 15000) break;
    delay(100);
  }

  for (int i = 0; i < 4; i++) {
    digitalWrite(redPins[i], HIGH);
  }
}
