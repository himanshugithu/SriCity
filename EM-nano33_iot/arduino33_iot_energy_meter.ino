#include <ModbusMaster.h>
#include <Arduino.h>
#include <WiFiNINA.h>  // Use WiFiNINA for Nano 33 IoT
#include "ctop_send.h"

// WiFi credentials
const char* ssid = "myssid";
const char* password = "password";

// Define RS485 control pins and UART for Nano 33 IoT
#define RS485_TX_PIN 1  // TX pin (may need adjustment based on wiring)
#define RS485_RX_PIN 0  // RX pin (may need adjustment based on wiring)
#define RS485_EN_PIN 4  // GPIO to control DE/RE (Driver Enable)
#define ledPin A3  // LED pin for status indication

// Create a ModbusMaster object
ModbusMaster node; 

// Global variables to store Modbus values
float voltage;
float current;
float frequency;
float power;
float energy;
float powerFactor;

// Enable/disable RS485 transmit/receive mode
void preTransmission() {
  digitalWrite(RS485_EN_PIN, HIGH);  // Enable transmitter
}

void postTransmission() {
  digitalWrite(RS485_EN_PIN, LOW);   // Enable receiver
}

// Helper function to convert two 16-bit Modbus registers into a float
float convertToFloat(uint16_t high, uint16_t low) {
  uint32_t combined = ((uint32_t)high << 16) | low;
  float result;
  memcpy(&result, &combined, sizeof(result));
  return result;
}

// Function to read all Modbus values and store them in global variables
void readModbusValues() {
  uint8_t result;

  // Read Voltage
  result = node.readHoldingRegisters(140, 2);  // Read from register 40141
  if (result == node.ku8MBSuccess) {
    voltage = convertToFloat(node.getResponseBuffer(1), node.getResponseBuffer(0));
  } else {
    voltage = 0.0;
    Serial.println("Failed to read voltage.");
  }
  delay(800);

  // Read Current
  result = node.readHoldingRegisters(148, 2);  // Read from register 40149
  if (result == node.ku8MBSuccess) {
    current = convertToFloat(node.getResponseBuffer(1), node.getResponseBuffer(0));
  } else {
    current = 0.0;
    Serial.println("Failed to read current.");
  }
  delay(800);

  // Read Frequency
  result = node.readHoldingRegisters(156, 2);  // Read from register 40157
  if (result == node.ku8MBSuccess) {
    frequency = convertToFloat(node.getResponseBuffer(1), node.getResponseBuffer(0));
  } else {
    frequency = 0.0;
    Serial.println("Failed to read frequency.");
  }
  delay(800);

  // Read Power
  result = node.readHoldingRegisters(100, 2);  // Read from register 40101
  if (result == node.ku8MBSuccess) {
    power = convertToFloat(node.getResponseBuffer(1), node.getResponseBuffer(0));
  } else {
    power = 0.0;
    Serial.println("Failed to read power.");
  }
  delay(800);

  // Read Energy
  result = node.readHoldingRegisters(158, 2);  // Read from register 40159
  if (result == node.ku8MBSuccess) {
    energy = convertToFloat(node.getResponseBuffer(1), node.getResponseBuffer(0));
  } else {
    energy = 0.0;
    Serial.println("Failed to read energy.");
  }
  delay(800);

  // Read Power Factor
  result = node.readHoldingRegisters(116, 2);  // Read from register 40117
  if (result == node.ku8MBSuccess) {
    powerFactor = convertToFloat(node.getResponseBuffer(1), node.getResponseBuffer(0));
  } else {
    powerFactor = 0.0;
    Serial.println("Failed to read power factor.");
  }
  delay(800);
}

void setup() {
  Serial.begin(115200);  // Start serial communication for debug
  pinMode(RS485_EN_PIN, OUTPUT);
  digitalWrite(RS485_EN_PIN, LOW);  // Start in receive mode
  pinMode(ledPin, OUTPUT);
  
  // Initialize WiFi for Nano 33 IoT
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, HIGH);  // Turn LED on
    delay(100);
    digitalWrite(ledPin, LOW);   // Turn LED off
    delay(100);
    Serial.print(".");  // Print dot to indicate progress
  }
  
  digitalWrite(ledPin, LOW); // Ensure LED is off
  Serial.println("Connected to WiFi");

  // Initialize UART for Modbus communication (Serial1)
  Serial1.begin(9600, SERIAL_8E1);  // 8E1 = 8 data bits, even parity, 1 stop bit
  node.begin(1, Serial1);  // Modbus slave ID = 1
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop() {
  // Check Wi-Fi connection status
  if (WiFi.status() == WL_CONNECTED) {
    // Read Modbus values and store them in global variables
    readModbusValues();

    // Print values to Serial Monitor
    Serial.println(".............................................");
    Serial.print("Voltage: "); Serial.println(voltage, 3);
    Serial.print("Current: "); Serial.println(current, 3);
    Serial.print("Frequency: "); Serial.println(frequency, 3);
    Serial.print("Power: "); Serial.println(power, 3);
    Serial.print("Energy: "); Serial.println(energy, 3);
    Serial.print("Power Factor: "); Serial.println(powerFactor, 3);
    Serial.println(".............................................");

    // Call postData function to send data
    postData(voltage, current, frequency, power, energy, powerFactor);
    
    delay(3000);  // Wait for 3 seconds before the next reading
  } else {
    // If Wi-Fi is not connected, blink the LED to indicate status
    digitalWrite(ledPin, HIGH);  // Turn LED on
    delay(100);
    digitalWrite(ledPin, LOW);   // Turn LED off
    delay(100);

    WiFi.begin(ssid, password);
  }
}
