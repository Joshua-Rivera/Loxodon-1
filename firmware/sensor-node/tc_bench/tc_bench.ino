#include <Wire.h>
#include <Adafruit_MCP9600.h>

// -----------------------------
// MCP9600 I2C PINS - UNO R3
// -----------------------------
#define MCP9600_SDA A4
#define MCP9600_SCL A5

#define MCP9600_ADDRESS 0x67

Adafruit_MCP9600 mcp;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("Starting MCP9600 test...");

  // Arduino Uno R3 hardware I2C:
  // SDA = A4
  // SCL = A5
  Wire.begin();

  if (!mcp.begin(MCP9600_ADDRESS)) {
    Serial.println("MCP9600 not found!");

    while (1) {
      delay(10);
    }
  }

  Serial.println("MCP9600 found!");

  // K-type thermocouple
  mcp.setThermocoupleType(MCP9600_TYPE_K);
}

void loop() {
  Serial.print("Thermocouple: ");
  Serial.print(mcp.readThermocouple());
  Serial.println(" C");

  Serial.print("Ambient: ");
  Serial.print(mcp.readAmbient());
  Serial.println(" C");

  Serial.println();

  delay(1000);
}