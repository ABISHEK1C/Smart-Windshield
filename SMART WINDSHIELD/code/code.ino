  #include <DHT.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions for DHT Sensor and Fan Relay
#define DHTPIN D4       // DHT11 connected to D4 on ESP8266
#define FAN_RELAY_PIN D5 // Fan relay connected to D5 on ESP8266
#define DHTTYPE DHT11   // DHT 11 sensor type

// Pin Definitions for Rain Sensor, Wiper Relay, and Servo
#define RAIN_SENSOR_PIN D1    // Rain sensor connected to GPIO D1
#define WIPER_RELAY_PIN D2    // Wiper relay control pin connected to GPIO D2
#define SERVO_PIN D3          // Servo signal pin connected to GPIO D3

// OLED I2C pins
#define SDA_PIN D6
#define SCL_PIN D7

DHT dht(DHTPIN, DHTTYPE);
Servo wiperServo;

void setup() {
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize Fan Relay Pin
  pinMode(FAN_RELAY_PIN, OUTPUT);
  digitalWrite(FAN_RELAY_PIN, HIGH); // Ensure fan is off at the start

  // Initialize Rain Sensor and Wiper Relay Pin
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(WIPER_RELAY_PIN, OUTPUT);
  digitalWrite(WIPER_RELAY_PIN, HIGH); // Wiper off (assumes active LOW relay)

  // Attach Servo to its pin and set initial position
  wiperServo.attach(SERVO_PIN);
  wiperServo.write(0); // Initial position (no movement)
  
  // Initialize OLED display
  Wire.begin(SDA_PIN, SCL_PIN); // Set custom I2C pins
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // I2C address for OLED
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  Serial.println("System initialized: Temperature control with fan and rain sensor with wiper.");
}

void loop() {
  // Temperature Control Section
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool fanOn = false;

  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    if (temp > 31) {
      digitalWrite(FAN_RELAY_PIN, LOW); // Turn on the fan
      Serial.println("Fan ON");
      fanOn = true;
    } else {
      digitalWrite(FAN_RELAY_PIN, HIGH); // Turn off the fan
      Serial.println("Fan OFF");
      fanOn = false;
    }
  }

  // Rain Sensor and Wiper Control Section
  int rainDetected = digitalRead(RAIN_SENSOR_PIN);
  bool wiperOn = false;

  if (rainDetected == LOW) { // Rain detected
    Serial.println("Rain detected, activating wiper.");
    digitalWrite(WIPER_RELAY_PIN, LOW); // Turn on wiper relay (active LOW)
    wiperOn = true;

    // Servo wiper movement (wider and faster sweeping motion)
    for (int pos = 0; pos <= 120; pos += 10) {
      wiperServo.write(pos);  // Move from 0 to 120 degrees
      delay(50); // Faster sweeping delay
    }
    for (int pos = 120; pos >= 0; pos -= 10) {
      wiperServo.write(pos);  // Move back from 120 to 0 degrees
      delay(50); // Faster sweeping delay
    }

  } else { // No rain detected
    Serial.println("No rain detected, turning off wiper.");
    digitalWrite(WIPER_RELAY_PIN, HIGH); // Turn off wiper relay
    wiperOn = false;
  }

  // Update OLED Display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("SMART WINDSHIELD");

  display.print("Temp: ");
  if (isnan(temp)) display.println("N/A");
  else display.print(temp); display.println(" C");

  display.print("Humidity: ");
  if (isnan(humidity)) display.println("N/A");
  else display.print(humidity); display.println(" %");

  if (rainDetected == LOW) {
    display.println("RAINING:");
    display.println("REAR WIPER ON!");
  } else {
    display.println("NO RAIN DETECTED");
    display.println("REAR WIPER OFF");
  }

  display.print("FAN: ");
  if (fanOn) display.println("ON");
  else display.println("OFF");

  display.display(); // Show display content
  delay(2000); // Delay before the next cycle
}
