#include <Arduino.h>
#include <esp_timer.h>
#include <math.h>
#include <stdio.h>

#include "pt_config.h"

#if !defined(CONFIG_IDF_TARGET_ESP32S3)
#error "Select an ESP32-S3 board; this draft targets the original UM FeatherS3."
#endif

struct Reading {
  bool valid;
  float psi;
};

uint64_t session_start_ms = 0;
uint64_t next_row_ms = 0;
bool configuration_valid = true;

uint64_t clockMs() {
  return static_cast<uint64_t>(esp_timer_get_time()) / 1000ULL;
}

bool channelConfigValid(const PtChannel& channel) {
  if (!channel.enabled) return true;
  return isfinite(channel.zero_volts) &&
         isfinite(channel.full_scale_volts) &&
         isfinite(channel.full_scale_psi) &&
         isfinite(channel.divider_top_ohms) &&
         isfinite(channel.divider_bottom_ohms) &&
         channel.zero_volts >= 0.0f &&
         channel.full_scale_volts > channel.zero_volts &&
         channel.full_scale_psi > 0.0f &&
         channel.divider_top_ohms >= 0.0f &&
         channel.divider_bottom_ohms > 0.0f;
}

// This is the nominal linear transfer function, not physical calibration.
float voltsToPsi(float sensor_volts, const PtChannel& channel) {
  return (sensor_volts - channel.zero_volts) * channel.full_scale_psi /
         (channel.full_scale_volts - channel.zero_volts);
}

Reading readPressure(size_t index, uint64_t elapsed_ms) {
  const PtChannel& channel = PT_CHANNELS[index];
  if (!channel.enabled) return {false, NAN};

  if (SIMULATE_INPUTS) {
    // Deterministic example data exercises formatting without any PT wiring.
    const float phase = static_cast<float>(elapsed_ms % 20000ULL) / 20000.0f;
    return {true, 500.0f + 25.0f * sinf(6.2831853f * phase + index * 0.2f)};
  }

  const uint32_t adc_mv = analogReadMilliVolts(channel.pin);
  // Outside the selected ESP32-S3 ADC measurement range: no pressure output.
  if (adc_mv < 150 || adc_mv >= 3100) return {false, NAN};

  const float adc_volts = adc_mv / 1000.0f;
  const float sensor_volts = adc_volts *
      (channel.divider_top_ohms + channel.divider_bottom_ohms) /
      channel.divider_bottom_ohms;

  // Keep out-of-transfer-range readings invalid rather than clipping to 0/1000.
  if (sensor_volts < channel.zero_volts ||
      sensor_volts > channel.full_scale_volts) return {false, NAN};

  const float psi = voltsToPsi(sensor_volts, channel);
  return {isfinite(psi), psi};
}

void formatElapsed(uint64_t ms, char* text, size_t length) {
  const unsigned long long hours = ms / 3600000ULL;
  const unsigned int minutes = (ms / 60000ULL) % 60;
  const unsigned int seconds = (ms / 1000ULL) % 60;
  const unsigned int fraction = ms % 1000ULL;
  snprintf(text, length, "%02llu:%02u:%02u.%03u", hours, minutes, seconds,
           fraction);
}

void printHeader() {
  if (CSV_OUTPUT) {
    Serial.println("Elapsed Time,PT1 (psi),PT2 (psi),PT3 (psi),PT4 (psi)");
  } else {
    Serial.println(" Elapsed Time  | PT1 (psi) | PT2 (psi) | PT3 (psi) | PT4 (psi)");
    Serial.println("---------------+-----------+-----------+-----------+-----------");
  }
}

void printRow(uint64_t elapsed_ms, const Reading* readings) {
  char elapsed[32];
  char value[40];
  formatElapsed(elapsed_ms, elapsed, sizeof(elapsed));
  if (CSV_OUTPUT) {
    Serial.print(elapsed);
    for (size_t i = 0; i < 4; ++i) {
      Serial.print(',');
      if (readings[i].valid) {
        snprintf(value, sizeof(value), "%.2f", readings[i].psi);
        Serial.print(value);
      }
    }
  } else {
    snprintf(value, sizeof(value), " %-13s ", elapsed);
    Serial.print(value);
    for (size_t i = 0; i < 4; ++i) {
      if (readings[i].valid) {
        snprintf(value, sizeof(value), "| %9.2f ", readings[i].psi);
      } else {
        snprintf(value, sizeof(value), "| %9s ", "--");
      }
      Serial.print(value);
    }
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  // Give USB a bounded connection window; do not wait forever for a monitor.
  const uint64_t wait_start = clockMs();
  while (!Serial && clockMs() - wait_start < 3000ULL) delay(10);

  for (size_t i = 0; i < 4; ++i) {
    configuration_valid = configuration_valid && channelConfigValid(PT_CHANNELS[i]);
  }
  if (!configuration_valid) {
    Serial.println("# ERROR: invalid PT configuration; acquisition stopped.");
    return;
  }

  if (!SIMULATE_INPUTS) {
    analogReadResolution(12);
    for (size_t i = 0; i < 4; ++i) {
      if (PT_CHANNELS[i].enabled) {
        analogSetPinAttenuation(PT_CHANNELS[i].pin, ADC_11db);
      }
    }
  }

  Serial.println(SIMULATE_INPUTS
      ? "# SIMULATION: generated examples, NOT pressure measurements."
      : "# DIRECT ADC: nominal PSI conversion, NOT calibrated pressure.");
  Serial.println("# Elapsed time marks scan start; four inputs are read sequentially.");
  printHeader();
  session_start_ms = clockMs();
  next_row_ms = session_start_ms;
}

void loop() {
  if (!configuration_valid) {
    delay(100);
    return;
  }
  const uint64_t scan_start = clockMs();
  if (scan_start < next_row_ms) {
    delay(1);
    return;
  }

  Reading readings[4];
  const uint64_t elapsed_ms = scan_start - session_start_ms;
  for (size_t i = 0; i < 4; ++i) readings[i] = readPressure(i, elapsed_ms);
  printRow(elapsed_ms, readings);

  // Skip missed slots instead of manufacturing backdated catch-up readings.
  const uint64_t after_row = clockMs();
  next_row_ms = session_start_ms +
      ((after_row - session_start_ms) / ROW_INTERVAL_MS + 1ULL) * ROW_INTERVAL_MS;
}
