#define LED_PIN_1 2  // LED on pin 2
#define LED_PIN_2 3  // LED on pin 3
#define LED_PIN_3 4  // LED on pin 4
#define LED_PIN_4 5  // LED on pin 5
#define BUTTON_PIN 6 // Non-latching button on pin 6
#define RELAY_PIN 9  // Relay on pin 9
#define POT_PIN A0   // Potentiometer on A0

const int ledPins[] = {LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4};
const int numLeds = 4;
const int flashDurationMin = 10; // Minimum flash duration (ms)
const int flashDurationMax = 50; // Maximum flash duration (ms)
const int offTime = 5;           // Off-time between LED flashes (ms)
const int pauseMin = 5;          // Minimum pause between flashes in surge (ms)
const int pauseMax = 20;         // Maximum pause between flashes in surge (ms)
const int toggleFlashDuration = 100; // Visual feedback flash (ms)
const int quickPressWindow = 1000;   // 1 second window for 3 presses (ms)

unsigned long lastSurgeTime = 0; // Time of last surge cycle
unsigned long lastButtonPress = 0; // Time of last button press
int pressCount = 0;              // Count of quick presses
bool relayOnDuringShow = false;  // false = OFF during show (normal), true = ON during show (toggled)
long interval = 100;             // Initial interval between surges (ms)

void setup() {
  // Initialize LED pins as outputs
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Start with LEDs off
  }
  // Initialize button with internal pull-up
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Initialize relay as output
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Start with relay ENERGIZED (button released)
  // Initialize potentiometer pin
  pinMode(POT_PIN, INPUT);
}

void loop() {
  // Read potentiometer (0-1023) and map to 100-2000ms (10-0.5 Hz)
  int potValue = analogRead(POT_PIN);
  interval = map(potValue, 0, 1023, 100, 2000);

  // Check button state (LOW = pressed)
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Button pressed - handle quick press detection
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress < quickPressWindow) {
      pressCount++;
      if (pressCount >= 3) {
        // 3 quick presses detected - TOGGLE relay state permanently
        relayOnDuringShow = !relayOnDuringShow;
        pressCount = 0; // Reset counter
        visualFeedback();
      }
    } else {
      pressCount = 1; // Start new count
    }
    lastButtonPress = currentTime;

    // Relay: HIGH when button released, or relayOnDuringShow when button pressed
    if (relayOnDuringShow) {
      digitalWrite(RELAY_PIN, HIGH); // ENERGIZED during light show (toggled ON)
    } else {
      digitalWrite(RELAY_PIN, LOW); // DE-ENERGIZED during light show (normal OFF)
    }

    // Check if it's time for a new surge cycle
    unsigned long currentTime2 = millis();
    if (currentTime2 - lastSurgeTime >= interval) {
      electricalSurge();
      lastSurgeTime = currentTime2;
      // Occasional sputter effect (30% chance), if button still pressed
      if (random(0, 100) < 30 && digitalRead(BUTTON_PIN) == LOW) {
        sputterFlash();
      }
    }
  } else {
    // Button released
    digitalWrite(RELAY_PIN, HIGH); // Relay ENERGIZED when button released (always)
  }

  // Minimal delay to prevent overloading
  delay(1);
}

void visualFeedback() {
  // All LEDs flash for 100ms to confirm toggle
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  delay(toggleFlashDuration);
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void electricalSurge() {
  // Random number of flashes in surge (2-5)
  int numFlashes = random(2, 6);
  for (int i = 0; i < numFlashes; i++) {
    // Stop if button released
    if (digitalRead(BUTTON_PIN) == HIGH) {
      return;
    }

    // Randomly select number of LEDs (1-4)
    int numActiveLeds = random(1, numLeds + 1);
    int activeLeds[numLeds];
    for (int j = 0; j < numLeds; j++) {
      activeLeds[j] = ledPins[j];
    }
    // Shuffle LEDs for random selection
    for (int j = 0; j < numActiveLeds; j++) {
      int k = random(j, numLeds);
      int temp = activeLeds[j];
      activeLeds[j] = activeLeds[k];
      activeLeds[k] = temp;
    }
    // Flash selected LEDs
    int flashDuration = random(flashDurationMin, flashDurationMax + 1);
    for (int j = 0; j < numActiveLeds; j++) {
      digitalWrite(activeLeds[j], HIGH);
    }
    delay(flashDuration); // Flash duration (10-50ms)
    for (int j = 0; j < numActiveLeds; j++) {
      digitalWrite(activeLeds[j], LOW);
    }
    delay(offTime); // Off-time to reset SCRs (5ms)
    // Pause between flashes in surge
    if (i < numFlashes - 1) {
      delay(random(pauseMin, pauseMax + 1)); // 5-20ms pause
    }
  }
}

void sputterFlash() {
  // Stop if button released
  if (digitalRead(BUTTON_PIN) == HIGH) {
    return;
  }

  // Single LED flicker for sputter effect
  int ledIndex = random(0, numLeds);
  int flashDuration = random(10, 21); // 10-20ms for subtle flicker
  digitalWrite(ledPins[ledIndex], HIGH);
  delay(flashDuration);
  digitalWrite(ledPins[ledIndex], LOW);
  delay(offTime); // Off-time to reset SCR (5ms)
}