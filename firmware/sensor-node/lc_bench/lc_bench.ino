#include <HX711_ADC.h>

const int DATA_PIN = 2;
const int CLOCK_PIN = 3;
const float CALIBRATION_FACTOR = 58.48;
const float GRAMS_TO_POUNDS = 0.00220462;

HX711_ADC loadCell(DATA_PIN, CLOCK_PIN);

void setup() {
  Serial.begin(9600);
  loadCell.begin();
  loadCell.start(2000, true);       // Stabilize for 2 seconds and tare.
  if (loadCell.getTareTimeoutFlag() || loadCell.getSignalTimeoutFlag()) {
    Serial.println("ERROR: check HX711 wiring");
    while (true) {}
  }
  loadCell.setCalFactor(CALIBRATION_FACTOR);
  Serial.println("millis,grams,pounds");
}

void loop() {
  if (loadCell.update()) {
    float grams = loadCell.getData();
    float pounds = grams * GRAMS_TO_POUNDS;

    Serial.print(millis());
    Serial.print(',');
    Serial.print(grams, 2);
    Serial.print(',');
    Serial.println(pounds, 3);
  }
}
