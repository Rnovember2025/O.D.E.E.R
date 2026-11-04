#include <Arduino.h>
#include <SCSCL.h> // Include the official Feetech SCS layer from the library

// Initialize Hardware Serial 2 on ESP32
HardwareSerial SerialServo(2); 

// Create a Feetech servo instance
SCSCL feetechServos;

const uint8_t SERVO_1_ID = 1;
const uint8_t SERVO_2_ID = 2;

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing ESP32 with PlatformIO Feetech Library...");

    // Initialize UART2: 1,000,000 Baud, RX=GPIO 16, TX=GPIO 17
    SerialServo.begin(1000000, SERIAL_8N1, 16, 17);
    
    // Pass your hardware serial configuration directly to the library instance
    feetechServos.pSerial = &SerialServo;
    delay(500);
}

void loop() {
    // 1. Move both servos using library functions
    // Arguments: WritePos(ID, Position, Speed, Acceleration)
    // Position range: 0 to 1023 | Speed range: 0 to 1023
    Serial.println("Sweeping to Position 200...");
    feetechServos.WritePos(SERVO_1_ID, 200, 150, 0); 
    feetechServos.WritePos(SERVO_2_ID, 200, 150, 0);
    delay(2000); 

    Serial.println("Sweeping to Position 800...");
    feetechServos.WritePos(SERVO_1_ID, 800, 150, 0);
    feetechServos.WritePos(SERVO_2_ID, 800, 150, 0);
    delay(2000);

    // 2. Reading Telemetry data (Checking if data lines work)
    // The library handles decoding return frames automatically!
    int currentPos1 = feetechServos.ReadPos(SERVO_1_ID);
    int currentPos2 = feetechServos.ReadPos(SERVO_2_ID);

    if (currentPos1 != -1) {
        Serial.printf("Telemetry -> Servo 1 Pos: %d | Servo 2 Pos: %d\n", currentPos1, currentPos2);
    } else {
        Serial.println("Telemetry Failure: Check your half-duplex hardware wires!");
    }
    delay(500);
}

