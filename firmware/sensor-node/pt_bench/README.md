# FeatherS3 pressure bench draft

Four-channel preliminary reader with the requested aligned terminal table and numeric CSV output. It defaults to labeled simulated data, so output can be reviewed before the measurement circuit is finalized.

## Run the draft

1. Open pt_bench.ino in Arduino IDE with pt_config.h in the same sketch folder.
2. Install Espressif's ESP32 Arduino board package and select the original Unexpected Maker FeatherS3. FeatherS3[D] is a different board variant and needs its own verified profile.
3. Enable USB CDC on boot where the board menu exposes that option. Select the connected port and upload deliberately.
4. Open the serial monitor at 115200 baud. Output starts after a bounded USB wait. Reset with the monitor open if you missed the header.

The initial mode reads no GPIO. A leading SIMULATION comment identifies example values. No firmware was uploaded as part of preparing this draft.

```text
 Elapsed Time  | PT1 (psi) | PT2 (psi) | PT3 (psi) | PT4 (psi)
---------------+-----------+-----------+-----------+-----------
 00:00:00.000  |    500.00 |    504.97 |    509.74 |    514.12
```

ASCII separators work in serial monitors without Unicode support. Values have two decimal places for readability; this does not imply 0.01 PSI measurement accuracy.

## Output settings

Edit pt_config.h:
- CSV_OUTPUT = false: aligned terminal table.
- CSV_OUTPUT = true: Elapsed Time,PT1 (psi),PT2 (psi),PT3 (psi),PT4 (psi).
- ROW_INTERVAL_MS = 1000: one four-channel scan per second.
- Each channel's enabled flag can hide an unconnected input.

Invalid/disabled channels appear as -- in the table and empty CSV cells. CSV mode emits startup comments beginning with # so simulation and nominal conversion are not mistaken for validated measurements. A future file exporter must retain that metadata and omit comment lines from the CSV table itself. This sketch emits serial text; it does not create a CSV file on a computer or SD card. Save only the header/data rows if importing the serial capture directly into a spreadsheet, and retain the startup comments separately.

Each row uses the actual elapsed scan-start time, not an invented rounded acquisition timestamp. The four inputs are measured sequentially, not simultaneously. Rows can be delayed by USB output. The draft performs no high-rate recording, averaging, RF batching, or missed-sample recovery. Full-rate acquisition and aligned export windows remain separate follow-up work.

## Before reading real sensors

Confirm these for the actual test setup:
- Exact board and PT models, sensor power requirement, output type, and pressure range.
- Whether the signal enters a divider and the onboard ADC, or an external ADS1115.
- Actual signal/ground connections, GPIO assignments, and measured divider ratio.

The direct-ADC example assumes, without confirmation, a linear 0–1000 PSI sensor with a 0.5–4.5 V output. It uses GPIO1/3/4/5 and illustrative 18k/10k dividers. These are editable examples, not the report's verified circuit. A 4.5 V sensor signal must not connect directly to a FeatherS3 GPIO. Verify ADC-junction voltage before connection; firmware cannot protect an overvoltage input.

For an explicitly verified direct-ADC setup, enter the actual per-channel values and set WIRING_VERIFIED and SENSOR_SPECIFICATIONS_VERIFIED to true before changing SIMULATE_INPUTS to false. The compile-time checks otherwise block hardware mode. If the setup uses ADS1115, replace the input-reading function with the correct external-ADC driver; changing GPIO numbers is insufficient.

The code uses the resistor convention: top is PT output to ADC junction; bottom is ADC junction to ground. It reconstructs sensor voltage, then applies:

```text
sensor_V = adc_V * (top_ohms + bottom_ohms) / bottom_ohms
psi = (sensor_V - zero_V) * full_scale_psi / (full_scale_V - zero_V)
```

This nominal conversion is not calibration. Test the entire chain against reference pressures. Inputs outside the configured transfer range or ADC measurement limits are invalid, rather than clipped to zero/full scale. A disconnected floating analog input can still produce a plausible value; this draft does not reliably diagnose disconnection. It does not measure sensor-supply variation or automatically correct a ratiometric sensor for it.

## References and validation

- [Espressif FeatherS3 variant](https://github.com/espressif/arduino-esp32/blob/master/variants/um_feathers3/pins_arduino.h): original-board ADC aliases and battery-sense pin.
- [Espressif ADC API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/adc.html): analogReadMilliVolts and attenuation measurement ranges.

Host C++ checks passed for table/CSV simulation, mocked direct-ADC conversion, elapsed-time formatting, invalid cells, missed-slot scheduling, and blocking unverified hardware mode. These checks use substituted Arduino/ADC functions. The installed Arduino environment does not currently contain an ESP32 board core, so this is not a verified FeatherS3 build. USB behavior, wiring, voltage accuracy, and calibrated pressure performance remain untested. Compile against the selected ESP32 board core and test hardware before use.
