# Telemetry protocol

Keep packet specifications and shared encoding/decoding examples here. Add formal schemas and test vectors with the implementation.

## Batching

Use one-second acquisition windows as the initial proposal, while preserving full-rate original samples. Window length does not set the sensor acquisition rate or require one physical RF packet.

Each batch identifies its source, boot, window, sample count, and acquisition times or offsets. Split large batches into numbered chunks with an expected chunk count. Specify versions, field sizes, byte order, units/scaling, faults, and size limits before implementation.

Persist received input immediately. Assemble complete batches separately, publishing after the window closes or a configured timeout. Mark missing chunks/sources explicitly; never insert zeros or wait indefinitely. Preserve late arrivals and define how exports are revised.

Provide raw-sample exports and aligned-window exports. An aligned value needs a defined summary rule, sample count, and completeness status. Keep acquisition time distinct from arrival time and window labels.

Multiple nodes require a verified common time reference or clock mapping. Batching alone does not synchronize them. Check total packet airtime before choosing transmission rates.
