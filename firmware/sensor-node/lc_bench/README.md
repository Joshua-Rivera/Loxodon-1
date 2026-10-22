# Load cell bench reader

Minimal Serial-only load-cell reader for one HX711 amplifier. It contains no Wi-Fi, SD-card, server, or credential code.

## Wiring

| HX711 signal | Arduino Uno pin |
|---|---:|
| DOUT | D2 |
| SCK | D3 |
| VCC | Compatible supply for the exact module |
| GND | GND |

Install the **HX711_ADC** library, upload the sketch, and open Serial Monitor at **9600 baud**.

Output is CSV whenever the HX711 library reports a new filtered measurement:

```text
millis,grams,pounds
2150,453.60,1.000
```

## Calibration status

The sketch uses `58.48`, copied from the [lc_calibrate](../lc_calibrate/lc_calibrate.ino) sketch (carried over from the previous repository). The previous repository's operational logger used a conflicting value of `42.15`; therefore this value must be treated as an assumption, not as verified calibration for the installed load cell.

`HX711_ADC::getData()` returns a filtered value scaled by the calibration factor. It only represents grams if the factor was established using gram-based reference masses. The pounds conversion is correct only after that condition is verified.

Calibrate the exact installed load cell and HX711 together:

1. Mount the load cell exactly as it will be used and remove all applied load.
2. Allow the electronics and structure to stabilize, then tare.
3. Apply a traceable known mass without shock loading.
4. Determine the calibration factor using the HX711_ADC calibration example.
5. Test several increasing and decreasing masses across the intended range.
6. Check zero return, repeatability, hysteresis, noise, and saturation.
7. Replace `CALIBRATION_FACTOR` with the verified result and repeat the test.

This program reports measurements only. It is not an independent safety or shutdown system.
