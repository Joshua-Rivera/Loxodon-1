// Binary post
#include <HX711_ADC.h>

// pins:
const int HX711_dout = 2; // HX711 DOUT -> D2
const int HX711_sck  = 3; // HX711 SCK  -> D3

// HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

// Threshold in lbs
const float threshold = 0.0f;

void setup() {
  Serial.begin(9600);
  delay(50);
  Serial.println("Initializing...");

  LoadCell.begin();

  float calibrationValue = 58.48; 
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;

  LoadCell.start(stabilizingtime, _tare);
  LoadCell.setCalFactor(calibrationValue);

  while (!Serial) {
    delay(10);
  }
  delay(200);
}

void loop() {
  // Always call update
  bool newData = LoadCell.update();

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 500) {
    float raw = LoadCell.getData();
    float lbs = raw * 0.00220462f;

    Serial.print("HX711 newData=");
    Serial.print(newData);
    Serial.print("  raw=");
    Serial.print(raw, 2);
    Serial.print("  lbs=");
    Serial.print(lbs, 2);
    Serial.println(" lbs");

    lastPrint = millis();
  }

  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
    else if(inByte == ']') LoadCell.setCalFactor(LoadCell.getCalFactor() + 1);
    else if(inByte == '[') LoadCell.setCalFactor(LoadCell.getCalFactor() - 1);
  }

  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}