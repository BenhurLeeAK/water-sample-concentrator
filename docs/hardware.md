# Hardware

## Concentrator (Module B)

| Component | Qty | Specification |
| --- | --- | --- |
| ESP32 microcontroller | 1 | Dual-core, Wi-Fi enabled |
| 12 V SMPS | 1 | 16.5 A output |
| Water pump (12 V) | 2 | Inlet and backflush |
| Dosing pump | 1 | Variable speed |
| Solenoid valve | 4 | 12 V DC, normally closed (SV1 to SV4) |
| Flow sensor | 3 | Hall effect, pulse output (YF-S201, K = 0.45 pulses/mL nominal) |
| 8-channel relay | 1 | 5 V logic, active LOW |
| Motor driver | 1 | Dual-channel PWM |

Also used: a custom concentration chamber with blast gates and a mesh filter, and a sample chamber.

Valve roles: SV1 inflow, SV2 outflow of low-concentration water to waste, SV3 backflush inflow, SV4 concentrate to the sample chamber and dosing pump.

## Microscope (Module A)

| Part | Detail |
| --- | --- |
| Objective | 45x / 0.65 NA finite-conjugate (160/0.17) |
| Focuser | Hadley-derived helical focuser, M56 x 5.5 thread, 72 mm travel |
| Mount | RMS adapter, 0.7965 in thread |
| Material | Black PLA, 0.4 mm nozzle, 0.2 mm layers, 30% infill, no supports |
| Thread tolerance used | 0.37 mm per side |
| Focus lock | 743 Flex Instant Glue (Metlok) |
| Camera | Raspberry Pi Global Shutter Camera (Sony IMX296, 1.6 MP) |
| Illumination | LED |

Total component cost of the concentrator is roughly US$75.
