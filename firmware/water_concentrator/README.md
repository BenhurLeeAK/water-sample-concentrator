# Firmware: ESP32 water-sample concentrator

`water_concentrator/water_concentrator.ino` is the control firmware for the concentrator described in the paper.

- Five stages: filling, settlement, water removal, backflush recovery, dosing.
- Three flow-sensor pulse counters on interrupt pins; volume = pulses / 0.45 pulses per mL.
- Web interface on the ESP32 access point: Start, Stop, live volumes, adjustable stage times and PWM values.
- Relays are active LOW. Inlet pump 255 (100 %) or 179 (70 %); dosing pump 80/255.

**Status of this file.** It is a reference implementation written from the control design in the paper, based on the authors' earlier time-based sketch. It has not been confirmed to be identical to the code that ran during the three reported trials. The flow-sensor pins (`SENSOR1_PIN` to `SENSOR3_PIN`) must be set to the actual wiring, and the Wi-Fi password must be changed before use.
