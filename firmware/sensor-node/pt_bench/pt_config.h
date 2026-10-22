#pragma once

#include <stdint.h>

// Default demo mode never reads an input pin. Its output is labeled SIMULATION.
constexpr bool SIMULATE_INPUTS = true;
constexpr bool CSV_OUTPUT = false;  // false: aligned table; true: numeric CSV
constexpr uint32_t ROW_INTERVAL_MS = 1000;

// Required before enabling hardware mode. These are not calibration approvals.
constexpr bool WIRING_VERIFIED = false;
constexpr bool SENSOR_SPECIFICATIONS_VERIFIED = false;

struct PtChannel {
  uint8_t pin;
  bool enabled;
  float zero_volts;
  float full_scale_volts;
  float full_scale_psi;
  float divider_top_ohms;     // PT signal to ADC junction
  float divider_bottom_ohms;  // ADC junction to ground
};

// EXAMPLES ONLY: hypothetical 0-1000 PSI, 0.5-4.5 V transducers.
// GPIO1/3/4/5 correspond to A0/A2/A3/A4 on the original UM FeatherS3.
// GPIO2 is intentionally avoided: the board uses it for battery sensing.
// The example 18k/10k divider is not a verified circuit or wiring instruction.
constexpr PtChannel PT_CHANNELS[] = {
    {1, true, 0.5f, 4.5f, 1000.0f, 18000.0f, 10000.0f},
    {3, true, 0.5f, 4.5f, 1000.0f, 18000.0f, 10000.0f},
    {4, true, 0.5f, 4.5f, 1000.0f, 18000.0f, 10000.0f},
    {5, true, 0.5f, 4.5f, 1000.0f, 18000.0f, 10000.0f},
};

static_assert(SIMULATE_INPUTS || WIRING_VERIFIED,
              "Confirm the actual input circuit before enabling real readings.");
static_assert(SIMULATE_INPUTS || SENSOR_SPECIFICATIONS_VERIFIED,
              "Confirm every transducer transfer function before hardware mode.");
static_assert(ROW_INTERVAL_MS > 0, "Row interval must be positive.");
static_assert(sizeof(PT_CHANNELS) / sizeof(PT_CHANNELS[0]) == 4,
              "The bench output expects four pressure channels.");
