const int lightPins[] = {2, 3, 4,5}; // SCR pins: 100W, 30W #1, 30W #2, 10W #1
const int numLights = 4;
const int potPin = A0; // Potentiometer
unsigned long lastEvent = 0; // Last event time
unsigned long lastPhantomGlow = 0; // Last phantom glow time
bool inEvent = false; // Track if in event

void setup() {
  for (int i = 0; i < numLights; i++) {
    pinMode(lightPins[i], OUTPUT);
    digitalWrite(lightPins[i], LOW);
  }
  pinMode(potPin, INPUT);
  randomSeed(analogRead(0));
}

void loop() {
  unsigned long currentTime = millis();
  int potValue = analogRead(potPin); // 0–1023
  long eventInterval = map(potValue, 0, 1023, 20000, 500); // 3s to 18s

  // Main event: Phantom pulse or shadow sweep
  if (!inEvent && currentTime - lastEvent >= eventInterval) {
    inEvent = true;
    int eventType = random(0, 2); // 0: Phantom pulse, 1: Shadow sweep

    if (eventType == 0) { // Phantom pulse (stuttering, eerie flashes)
      int numPulses = random(3, 6); // 3–7 pulses
      int targetLight = random(0, numLights); // Focus on one light
      for (int i = 0; i < numPulses; i++) {
        digitalWrite(lightPins[targetLight], HIGH);
        delay(random(15, 46)); // 15–45 ms
        digitalWrite(lightPins[targetLight], LOW);
        delay(random(80, 201)); // 80–200 ms dark, stuttering effect
        if (random(0, 3) == 0) { // 33% chance of secondary flicker
          int otherLight = random(0, numLights);
          digitalWrite(lightPins[otherLight], HIGH);
          delay(random(10, 31));
          digitalWrite(lightPins[otherLight], LOW);
        }
      }
    } else { // Shadow sweep (sequential, wave-like)
      int numSweeps = random(2, 6); // 2–4 sweeps
      int order[] = {0, 1, 2, 3};
      for (int i = numLights - 1; i > 0; i--) { // Shuffle order
        int j = random(0, i + 1);
        int temp = order[i];
        order[i] = j;
        order[j] = temp;
      }
      for (int i = 0; i < numSweeps; i++) {
        for (int j = 0; j < numLights; j++) {
          digitalWrite(lightPins[order[j]], HIGH);
          delay(random(30, 120)); // 30–80 ms, flowing
          digitalWrite(lightPins[order[j]], LOW);
        }
        delay(random(100, 501)); // 100–300 ms dark between sweeps
      }
    }

    lastEvent = currentTime;
    lastPhantomGlow = currentTime; // Reset glow timer
    inEvent = false;
  }

  // Phantom glow (subtle, random single flash)
  if (!inEvent && currentTime - lastPhantomGlow >= random(1500, eventInterval - 1500)) {
    int randomLight = random(0, numLights);
    digitalWrite(lightPins[randomLight], HIGH);
    delay(random(20, 80)); // 20–50 ms faint glow
    digitalWrite(lightPins[randomLight], LOW);
    lastPhantomGlow = currentTime;
  }
}