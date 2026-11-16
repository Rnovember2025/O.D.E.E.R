#include <Arduino.h>
#include <Wire.h>

#define I2C_DEV_ADDR 0x55
#define SDA_PIN 21
#define SCL_PIN 22
#define LED_PIN 2

uint32_t i = 0;

// Runs when the master requests data from this ESP32
void onRequest() {
  digitalWrite(LED_PIN, HIGH);

  Wire.print(i++);
  Wire.print(" Packets.");
  Serial.println("onRequest triggered");
}

// Runs when the master sends data to this ESP32
void onReceive(int len) {
  // digitalWrite(LED_PIN, HIGH);
  // delay(100);
  // digitalWrite(LED_PIN, LOW);
  Serial.printf("onReceive[%d]: ", len);
  while (Wire.available()) {
    Serial.write(Wire.read());
  }
  Serial.println();
  
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  // Register callbacks BEFORE calling Wire.begin()
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  
  // Initialize as I2C Slave
  Wire.begin((uint8_t)I2C_DEV_ADDR, SDA_PIN, SCL_PIN, 100000);
}

void loop() {
  // Keep loop non-blocking; I2C events are handled via interrupts/callbacks
  delay(100);
}
