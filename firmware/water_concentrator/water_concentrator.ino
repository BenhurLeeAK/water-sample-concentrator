/*
  ESP32 water-sample concentrator: reference firmware
  ---------------------------------------------------
  Written for this repository from the control design described in the paper
  (five-stage sequence, three YF-S201 pulse counters, web interface with
  start/stop and adjustable stage times). It is based on the authors' earlier
  time-based sketch and keeps its pin map and PWM values.

  IMPORTANT: this is a reference implementation. It has NOT been verified
  against the exact code that ran during the three trials in the paper.
  Check every pin below against your wiring before use.

  Board: ESP32 Dev Module, Arduino-ESP32 core 3.x (uses analogWrite).
  Library: WiFi and WebServer (both included with the core).

  Stages: 1 Fill -> 2 Settlement -> 3 Water removal -> 4 Backflush -> 5 Dosing
  Relays are active LOW. Volumes are V = N / K with K = 0.45 pulses/mL
  (nominal YF-S201, no per-sensor calibration).
*/
#include <WiFi.h>
#include <WebServer.h>

// ---- Wi-Fi access point (change the password before use) ----
const char* AP_SSID = "WaterSystem";
const char* AP_PASS = "change-this-password";

// ---- Pin map (valves, backflush pump, motor driver) ----
#define VALVE1 26   // SV1 inlet
#define VALVE2 27   // SV2 drain of low-concentration water
#define VALVE3 13   // SV3 backflush
#define VALVE4 21   // SV4 backflush return path
#define PUMP2  22   // backflush pump (relay)

#define ENA 14      // dosing pump PWM
#define IN1 18
#define IN2 19
#define ENB 23      // inlet pump PWM
#define IN3 4
#define IN4 5

// ---- Flow-sensor pins: SET THESE TO YOUR WIRING ----
#define SENSOR1_PIN 32   // sensor 1: inlet
#define SENSOR2_PIN 33   // sensor 2: water removal
#define SENSOR3_PIN 25   // sensor 3: backflush / dosing

const float K_PULSES_PER_ML = 0.45f;   // nominal YF-S201 constant

// ---- Adjustable settings (seconds, PWM duty 0-255) ----
struct Settings {
  float fillS = 17.0f;      // stage 1
  float settleS = 1.5f;     // stage 2
  float removeS = 5.0f;     // stage 3
  float backflushS = 8.0f;  // stage 4
  float dosingS = 8.0f;     // stage 5
  int inletPwm = 255;       // 255 = 100 %, 179 = 70 %
  int dosingPwm = 80;       // 80/255
} cfg;

enum Stage { IDLE, FILL, SETTLE, REMOVE, BACKFLUSH, DOSING, DONE };
const char* STAGE_NAME[] = {"Idle", "1 Filling", "2 Settlement",
                            "3 Water removal", "4 Backflush recovery",
                            "5 Dosing", "Completed"};

WebServer server(80);
volatile uint32_t pulses[3] = {0, 0, 0};
Stage stage = IDLE;
uint32_t stageStartMs = 0;
uint32_t cycleStartMs = 0;
uint32_t cycleMs = 0;

void IRAM_ATTR isr1() { pulses[0]++; }
void IRAM_ATTR isr2() { pulses[1]++; }
void IRAM_ATTR isr3() { pulses[2]++; }

void relayOn(int p)  { digitalWrite(p, LOW); }
void relayOff(int p) { digitalWrite(p, HIGH); }

void inletPump(bool on) {
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENB, on ? cfg.inletPwm : 0);
}
void dosingPump(bool on) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  analogWrite(ENA, on ? cfg.dosingPwm : 0);
}

void allOff() {
  inletPump(false); dosingPump(false);
  relayOff(VALVE1); relayOff(VALVE2); relayOff(VALVE3);
  relayOff(VALVE4); relayOff(PUMP2);
}

float volumeMl(int i) {
  noInterrupts(); uint32_t n = pulses[i]; interrupts();
  return n / K_PULSES_PER_ML;
}

void enterStage(Stage s) {
  allOff();
  stage = s;
  stageStartMs = millis();
  switch (s) {
    case FILL:      relayOn(VALVE1); relayOn(VALVE2); inletPump(true); break;
    case SETTLE:    break;                                   // everything closed
    case REMOVE:    relayOn(VALVE2); break;                  // drain by gravity
    case BACKFLUSH: relayOn(VALVE3); relayOn(VALVE4); relayOn(PUMP2); break;
    case DOSING:    dosingPump(true); break;
    case DONE:      cycleMs = millis() - cycleStartMs; break;
    default: break;
  }
}

void startCycle() {
  noInterrupts(); pulses[0] = pulses[1] = pulses[2] = 0; interrupts();
  cycleStartMs = millis();
  enterStage(FILL);
}

void stopCycle() { enterStage(IDLE); }

float stageLimitS(Stage s) {
  switch (s) {
    case FILL: return cfg.fillS;
    case SETTLE: return cfg.settleS;
    case REMOVE: return cfg.removeS;
    case BACKFLUSH: return cfg.backflushS;
    case DOSING: return cfg.dosingS;
    default: return 0;
  }
}

void runSequence() {
  if (stage == IDLE || stage == DONE) return;
  if ((millis() - stageStartMs) >= (uint32_t)(stageLimitS(stage) * 1000.0f)) {
    enterStage((Stage)(stage + 1));
  }
}

String statusJson() {
  String j = "{\"stage\":\"" + String(STAGE_NAME[stage]) + "\"";
  j += ",\"v1\":" + String(volumeMl(0), 1);
  j += ",\"v2\":" + String(volumeMl(1), 1);
  j += ",\"v3\":" + String(volumeMl(2), 1);
  j += ",\"cycle_s\":" + String(
      ((stage == IDLE) ? 0 : (stage == DONE ? cycleMs : millis() - cycleStartMs)) / 1000.0f, 1);
  j += "}";
  return j;
}

void handleRoot() {
  String p = "<html><head><meta name='viewport' content='width=device-width'>";
  p += "<meta http-equiv='refresh' content='2'></head><body>";
  p += "<h2>ESP32 water concentrator</h2>";
  p += "<p><b>Stage:</b> " + String(STAGE_NAME[stage]) + "</p>";
  p += "<p>Sensor 1 (inlet): " + String(volumeMl(0), 1) + " mL<br>";
  p += "Sensor 2 (removal): " + String(volumeMl(1), 1) + " mL<br>";
  p += "Sensor 3 (backflush): " + String(volumeMl(2), 1) + " mL</p>";
  p += "<form action='/start'><button>Start</button></form> ";
  p += "<form action='/stop'><button>Stop</button></form>";
  p += "<h3>Settings</h3><form action='/set'>";
  p += "Fill (s) <input name='fill' value='" + String(cfg.fillS, 1) + "'><br>";
  p += "Settle (s) <input name='settle' value='" + String(cfg.settleS, 1) + "'><br>";
  p += "Removal (s) <input name='remove' value='" + String(cfg.removeS, 1) + "'><br>";
  p += "Backflush (s) <input name='back' value='" + String(cfg.backflushS, 1) + "'><br>";
  p += "Dosing (s) <input name='dose' value='" + String(cfg.dosingS, 1) + "'><br>";
  p += "Inlet PWM (0-255) <input name='ipwm' value='" + String(cfg.inletPwm) + "'><br>";
  p += "Dosing PWM (0-255) <input name='dpwm' value='" + String(cfg.dosingPwm) + "'><br>";
  p += "<button>Save</button></form></body></html>";
  server.send(200, "text/html", p);
}

void redirectHome() {
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSet() {
  if (stage != IDLE && stage != DONE) { redirectHome(); return; }  // not while running
  auto f = [](const char* n, float cur, float lo, float hi) {
    if (!server.hasArg(n)) return cur;
    float v = server.arg(n).toFloat();
    return (v < lo || v > hi) ? cur : v;
  };
  cfg.fillS = f("fill", cfg.fillS, 0, 600);
  cfg.settleS = f("settle", cfg.settleS, 0, 600);
  cfg.removeS = f("remove", cfg.removeS, 0, 600);
  cfg.backflushS = f("back", cfg.backflushS, 0, 600);
  cfg.dosingS = f("dose", cfg.dosingS, 0, 600);
  cfg.inletPwm = (int)f("ipwm", cfg.inletPwm, 0, 255);
  cfg.dosingPwm = (int)f("dpwm", cfg.dosingPwm, 0, 255);
  redirectHome();
}

void setup() {
  Serial.begin(115200);

  int outs[] = {VALVE1, VALVE2, VALVE3, VALVE4, PUMP2, ENA, IN1, IN2, ENB, IN3, IN4};
  for (int p : outs) pinMode(p, OUTPUT);
  allOff();

  pinMode(SENSOR1_PIN, INPUT_PULLUP);
  pinMode(SENSOR2_PIN, INPUT_PULLUP);
  pinMode(SENSOR3_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SENSOR1_PIN), isr1, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2_PIN), isr2, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR3_PIN), isr3, FALLING);

  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/status", []() { server.send(200, "application/json", statusJson()); });
  server.on("/start", []() { if (stage == IDLE || stage == DONE) startCycle(); redirectHome(); });
  server.on("/stop", []() { stopCycle(); redirectHome(); });
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
  runSequence();
}
