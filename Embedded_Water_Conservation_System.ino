#define FLOW_SENSOR 2
#define RELAY_PIN 8
#define BUTTON_PIN 7

volatile unsigned long pulseCount = 0;

unsigned long lastPrintTime = 0;
unsigned long flowStartTime = 0;

bool motorState = true;
bool flowActive = false;

bool lastButtonState = HIGH;

void countPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);

  pinMode(FLOW_SENSOR, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR), countPulse, RISING);

  digitalWrite(RELAY_PIN, LOW);

  Serial.println("System Started - Motor ON");
}

void loop() {

  if (millis() - lastPrintTime >= 1000) {

    lastPrintTime = millis();

    unsigned long currentPulses = pulseCount;
    pulseCount = 0;

    Serial.print("Pulse Count: ");
    Serial.print(currentPulses);

    // 🔹 Adjusted threshold for your real flow
    if (currentPulses > 3 && motorState) {

      if (!flowActive) {
        flowActive = true;
        flowStartTime = millis();
        Serial.print(" | Water Flow Started");
      }

      unsigned long flowSeconds = (millis() - flowStartTime) / 1000;

      Serial.print(" | Flow Time (sec): ");
      Serial.print(flowSeconds);

      if (flowSeconds >= 15) {
        digitalWrite(RELAY_PIN, HIGH);
        motorState = false;
        flowActive = false;
        Serial.print(" | OVERFLOW! Motor OFF");
      }

    } else {
      flowActive = false;
      Serial.print(" | No Flow");
    }

    Serial.println();
  }

  // -------- BUTTON --------
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(200);

    if (!motorState) {
      digitalWrite(RELAY_PIN, LOW);
      motorState = true;
      flowActive = false;
      Serial.println("Motor Restarted by Button");
    }
  }

  lastButtonState = currentButtonState;
}
