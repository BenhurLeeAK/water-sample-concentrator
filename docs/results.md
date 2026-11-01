# Results

All values below come from the paper. "Measured" means read from the sensors or a stopwatch; "calculated" means derived from measured values or from the stated pump displacement.

## Volume reduction (measured, 3 trials, 600 mL feed)

Volumes use V = N / K with the nominal K = 0.45 pulses/mL, without per-sensor calibration.

| Parameter | Trial 1 | Trial 2 | Trial 3 | Average |
| --- | --- | --- | --- | --- |
| Inlet volume (mL) | 600 | 600 | 600 | 600 |
| Water removed (mL) | 350 | 340 | 355 | 348 |
| Settled concentrate (mL) | 250 | 260 | 245 | 252 |
| C_f,chamber | 2.40 | 2.31 | 2.45 | 2.38 |
| Backflush, estimated (mL) | 36.5 | 36.5 | 36.5 | 36.5 |
| Final concentrate (mL) | 286.5 | 296.5 | 281.5 | 288.5 |
| C_f,eff | 2.09 | 2.02 | 2.13 | 2.08 |

## Stage timing (measured)

| Stage | Condition | Time (s) |
| --- | --- | --- |
| 1 Fill 600 mL | Inlet pump 100% PWM (947 mL/min) | 38.0 |
| 2 Settle | Mesh / baffle hold | 1.5 |
| 3 Drain 348 mL | SV2 fully open (about 949 mL/min) | 22.0 |
| 4 Backflush | Pump on, SV3 open | 8.0 |
| Total active cycle | | 69.5 |
| 5 Dose 288.5 mL | 31% PWM (37.2 mL/min) | 465 |

## Pump operating points (measured)

| Pump | PWM duty | Flow (mL/min) |
| --- | --- | --- |
| Inlet | 100% | 947.0 |
| Inlet | 70% | 661.1 |
| Dosing | 31% (80/255) | 37.2 |

## Effect of a sensor mismatch on the concentration factor (calculated)

Mismatch of Flow Sensor 2 relative to Flow Sensor 1, using the trial averages (V_in = 600 mL, V_removed = 348 mL) and the 3.20% maximum error reported for a calibrated YF-S201.

| Mismatch | V_removed (mL) | V_settled (mL) | C_f,chamber | C_f,eff (V_flush = 36.5 mL) |
| --- | --- | --- | --- | --- |
| -3.2% | 336.9 | 263.1 | 2.28 | 2.00 |
| -1% | 344.5 | 255.5 | 2.35 | 2.05 |
| 0 | 348.0 | 252.0 | 2.38 | 2.08 |
| +1% | 351.5 | 248.5 | 2.41 | 2.11 |
| +3.2% | 359.1 | 240.9 | 2.49 | 2.16 |

## Backflush duration trade-off (calculated)

Extrapolated from the 4.56 mL/s pump displacement and the 252 mL average settled volume. Only the 8 s row was tested, and solids recovery was not measured.

| Backflush (s) | V_flush (mL) | V_conc,final (mL) | Dilution (%) | C_f,eff | Active cycle (s) |
| --- | --- | --- | --- | --- | --- |
| 0 | 0 | 252.0 | 0 | 2.38 | 61.5 |
| 4 | 18.2 | 270.2 | 6.7 | 2.22 | 65.5 |
| 8 (as tested) | 36.5 | 288.5 | 12.6 | 2.08 | 69.5 |
| 12 | 54.7 | 306.7 | 17.8 | 1.96 | 73.5 |
| 16 | 73.0 | 325.0 | 22.5 | 1.85 | 77.5 |

## Key equations

- Concentration factor: C_f = V_in / V_conc
- Volume from pulses: V = N / K, with K = 0.45 pulses/mL, so one pulse is about 2.2 mL
- Volume balance: V_in + V_flush = V_removed + V_conc
- Abbe limit: d = lambda / (2 NA), about 412 nm for lambda = 535 nm and NA = 0.65
- Depth of field: lambda * n / NA^2, about 1.3 um
