// Pin assignments
const int sensorPins[4] = {A0, A1, A2, A3};
const int greenPins[4] = {3, 6, 9, 12};
const int yellowPins[4] = {4, 7, 10, 13};
// Using A4 as a digital pin for the last red light is acceptable,
// but for clarity and consistency, a dedicated digital pin is often preferred if available.
const int redPins[4] = {5, 8, 11, A4};
const int emergencyPin = 2;

// Traffic control timings (ms)
const unsigned long baseGreen = 10000;
const unsigned long maxGreen = 30000;
const unsigned long yellowTime = 3000;
const unsigned long allRedTime = 2000; // Time for all lights to be red between cycles
const unsigned long emergencyHoldTime = 15000; // How long to clear intersection in emergency
const unsigned long debounceDelay = 50; // Debounce time for buttons

// Sensor & traffic thresholds
const int sensorThreshold = 300; // Not directly used in current density logic, but good to have
const int heavyThreshold = 600;

// Global variable to keep track of the currently active lane
int currentActiveLane = -1; // -1 indicates no lane is active initially

// --- Function Declarations ---
void handleEmergency();
void cycleSignal(int lane, unsigned long greenTime);
void setLights(int lane, bool green, bool yellow, bool red);
void allLightsRed();

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(sensorPins[i], INPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(yellowPins[i], OUTPUT);
    pinMode(redPins[i], OUTPUT);
  }
  pinMode(emergencyPin, INPUT_PULLUP); // Use internal pull-up

  allLightsRed(); // Start all red initially
}

void loop() {
  // Simple debouncing for emergency pin
  static unsigned long lastEmergencyDebounceTime = 0;
  static int lastEmergencyButtonState = HIGH;
  int currentEmergencyButtonState = digitalRead(emergencyPin);

  if (currentEmergencyButtonState != lastEmergencyButtonState) {
    lastEmergencyDebounceTime = millis();
  }

  if ((millis() - lastEmergencyDebounceTime) > debounceDelay) {
    if (currentEmergencyButtonState == LOW && lastEmergencyButtonState == HIGH) { // Only trigger on a transition from HIGH to LOW
      handleEmergency();
    }
  }
  lastEmergencyButtonState = currentEmergencyButtonState;


  // If still in emergency mode (button held down and within emergency window),
  // handleEmergency would keep being called if it returned to loop immediately.
  // The current handleEmergency() has a blocking wait, which simplifies this.
  // However, if we make handleEmergency non-blocking, this check would be important.
  // For now, it's ok because handleEmergency blocks until emergency is 'cleared' or timed out.


  int densities[4];
  for (int i = 0; i < 4; i++) {
    densities[i] = analogRead(sensorPins[i]);
    Serial.print("Lane "); Serial.print(i + 1); // For user-friendly output
    Serial.print(": "); Serial.println(densities[i]);
  }

  int maxLane = 0;
  for (int i = 1; i < 4; i++) {
    if (densities[i] > densities[maxLane]) {
      maxLane = i;
    }
  }

  // If the currently active lane is still the busiest, extend its green time,
  // or apply the calculated green time directly. This avoids unnecessary cycling
  // if the same lane remains dominant.
  if (maxLane == currentActiveLane) {
      // Logic for extending green for the same lane could go here,
      // but for simplicity, we'll just re-evaluate and apply.
      // The current cycleSignal implementation will handle turning off and on lights correctly.
  }


  unsigned long greenTime = baseGreen;
  if (densities[maxLane] > heavyThreshold) {
    // Ensure map function doesn't produce values outside baseGreen to maxGreen
    greenTime = map(constrain(densities[maxLane], heavyThreshold, 1023), heavyThreshold, 1023, baseGreen, maxGreen);
    greenTime = min(greenTime, maxGreen); // Redundant after constrain, but good for clarity
  }

  cycleSignal(maxLane, greenTime);
}

// Helper function to set lights for a specific lane
void setLights(int lane, bool green, bool yellow, bool red) {
  digitalWrite(greenPins[lane], green ? HIGH : LOW);
  digitalWrite(yellowPins[lane], yellow ? HIGH : LOW);
  digitalWrite(redPins[lane], red ? HIGH : LOW);
}

// Helper function to set all lights to red
void allLightsRed() {
  for (int i = 0; i < 4; i++) {
    setLights(i, LOW, LOW, HIGH); // Green=OFF, Yellow=OFF, Red=ON
  }
}

void cycleSignal(int newActiveLane, unsigned long greenTime) {
  // If the new active lane is the same as the current one,
  // we might want to extend the green rather than cycle.
  // For now, this function will perform a full cycle.
  // To avoid flicker and ensure proper sequence:

  if (currentActiveLane != -1 && currentActiveLane != newActiveLane) {
    // If there was a previously active lane and it's different from the new one
    setLights(currentActiveLane, LOW, HIGH, LOW); // Old lane to Yellow
    delay(yellowTime);
    setLights(currentActiveLane, LOW, LOW, HIGH); // Old lane to Red
    delay(allRedTime); // Short all-red period for intersection clearing
  } else if (currentActiveLane == -1) {
    // First run or after emergency, ensure all are red before first green
    allLightsRed();
    delay(allRedTime); // Initial all-red
  } else if (currentActiveLane == newActiveLane) {
    // If the same lane is still the max, just continue its green.
    // The previous delay(greenTime) already handled this.
    // We just return and let the loop re-evaluate.
    // This assumes loop calls cycleSignal after a delay.
    // If we want to extend, the logic needs to be outside this function
    // or this function needs to be re-thought to manage state better.
    // For now, it will simply re-set the lights and apply delay.
  }


  // Now activate the new active lane
  setLights(newActiveLane, HIGH, LOW, LOW); // New lane to Green
  currentActiveLane = newActiveLane; // Update global tracking
  delay(greenTime); // Hold green for calculated time

  // After green, transition this lane to yellow then red
  setLights(currentActiveLane, LOW, HIGH, LOW); // Current lane to Yellow
  delay(yellowTime);
  setLights(currentActiveLane, LOW, LOW, HIGH); // Current lane to Red
  delay(allRedTime); // Short all-red period before next cycle starts
}


void handleEmergency() {
  Serial.println("Emergency detected! Clearing intersection...");

  allLightsRed(); // Set all lights to red during emergency clearing
  // Consider flashing all yellow/red during emergency, but all red is a safe default.

  unsigned long startTime = millis();
  // Wait for the emergency button to be released OR for a timeout
  while (digitalRead(emergencyPin) == LOW && (millis() - startTime) < emergencyHoldTime) {
    // Optionally flash all red lights here to indicate emergency
    // Example:
    // for(int i=0; i<4; i++) digitalWrite(redPins[i], HIGH);
    // delay(250);
    // for(int i=0; i<4; i++) digitalWrite(redPins[i], LOW);
    // delay(250);
    // This would make it non-blocking for a short flash, but the main loop is paused.
    // For a simple blocking emergency, a small delay is fine.
    delay(50); // Small delay to prevent busy-waiting too aggressively
  }

  // After emergency state, ensure all are red and reset active lane
  allLightsRed();
  currentActiveLane = -1; // Reset active lane state
  Serial.println("Emergency cleared. Resuming normal operation.");
}
