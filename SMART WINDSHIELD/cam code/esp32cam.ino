const int irSensorPin = 2;   // IR sensor output connected to pin 2
const int relayPin = 3;      // Relay input pin connected to pin 3

void setup() {
  Serial.begin(9600);          // Begin serial communication for debugging
  pinMode(irSensorPin, INPUT);  // IR sensor as input
  pinMode(relayPin, OUTPUT);    // Relay as output
  digitalWrite(relayPin, LOW);  // Start with relay off
}

void loop() {
  int irState = digitalRead(irSensorPin);
  Serial.println(irState);      // Print IR sensor state for debugging

  if (irState == HIGH) {        // Object detected
    digitalWrite(relayPin, HIGH);  // Turn on relay (and motor)
    Serial.println("Motor ON");
  } else {                       // No object detected
    digitalWrite(relayPin, LOW);   // Turn off relay (and motor)
    Serial.println("Motor OFF");
  }

  delay(100);  // Small delay for stable reading
}
