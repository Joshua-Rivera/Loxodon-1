# Loxodon-1

Avionics firmware and ground software for Loxodon-1.

## Structure

```text
firmware/
  sensor-node/       Onboard and GSE acquisition; share code, vary configuration
  receiver/          RF reception and USB forwarding
ground-station/      Python acquisition, recording, API, replay, and exports
dashboard/           Streamlit browser interface
protocol/            Packet definitions, timing, and shared test examples
config/              Channel maps, operating profiles, and calibrations
docs/                Hardware references, setup instructions, and test records
infra/               Optional services (Grafana)
```

Keep tests beside the code they exercise. Add internal folders when real code needs them.

## Software direction

- Firmware: C++, Arduino-ESP32, PlatformIO, RadioLib.
- Ground station: Python, pySerial, FastAPI, SQLite, and CSV exports.
- Dashboard: Python, Streamlit, reading SQLite read-only; reachable by SoftAP clients.
- Grafana (optional, infra/grafana/): reviews the same SQLite file via the frser-sqlite-datasource plugin and can be embedded in Streamlit. No InfluxDB.

The selected hardware follows the project reports. FlySky control, Featherweight recovery/tracking, and Walksnail video retain their own roles. Document their setup under docs/; add adapters only when implemented.

See [source chronology](docs/source-register.md) and [packet batching](protocol/README.md).

## Working rules

Preserve actual sample timestamps and calibration provenance. Keep recording independent of the browser. Store full sessions and video outside Git, with durable backups. Small fixtures and calibration reference CSV files can be committed.

The first preliminary sketch is the [FeatherS3 four-channel PT bench draft](firmware/sensor-node/pt_bench/README.md). It defaults to simulated readings until the input circuit is confirmed. The rest remains a scaffold. Add dependency manifests, lockfiles, build configuration, and automated checks alongside working code.

The existing [GPLv3 license](LICENSE) is retained.
