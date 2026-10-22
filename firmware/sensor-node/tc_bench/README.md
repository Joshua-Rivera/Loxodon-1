# Thermocouple bench reader

Minimal Serial-only reader for one K-type thermocouple on an Adafruit MCP9600 I2C breakout. Carried over from the previous repository; it contains no Wi-Fi, SD-card, server, or credential code.

## Wiring

| MCP9600 signal | Arduino Uno pin |
|---|---:|
| SDA | A4 |
| SCL | A5 |
| VIN | Compatible supply for the exact breakout |
| GND | GND |

The sketch expects I2C address `0x67`. Install the **Adafruit MCP9600** library, upload the sketch, and open Serial Monitor at **9600 baud**. It halts with `MCP9600 not found!` if the breakout does not respond.

Output is human-readable text once per second, not CSV:

```text
Thermocouple: 24.75 C
Ambient: 23.50 C
```

## Status

- Targets an Arduino Uno, not the FeatherS3 used by [pt_bench](../pt_bench/README.md). Porting is follow-up work.
- The MCP9600 performs its own cold-junction compensation, but the installed measurement chain is not calibrated by this code. Compare against a traceable reference across the operating range before use.
