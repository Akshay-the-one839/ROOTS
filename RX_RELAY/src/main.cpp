#include <Arduino.h>
#include <WiFi.h>

WiFiServer server(80);
WiFiClient client;

#define LED_PIN 2
unsigned long blinkTime     = 0;
bool ledOn                  = false;

unsigned long lastActivity  = 0;
unsigned long lastKeepAlive = 0;
const unsigned long CLIENT_TIMEOUT     = 15000;
const unsigned long KEEPALIVE_INTERVAL = 5000;

void blinkLED() { digitalWrite(LED_PIN, LOW); blinkTime = millis(); ledOn = true; }

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // ✅ Wait for PIC boot messages to finish, then flush
  delay(2000);
  while(Serial.available()) Serial.read();

  WiFi.softAP("ESP_RX", "12222223");
  server.begin();
}

void loop() {

  if (ledOn && millis() - blinkTime > 30) {
    digitalWrite(LED_PIN, HIGH);
    ledOn = false;
  }

  // ── Accept new TX ESP connection ──
  if (!client || !client.connected()) {
    client.stop();
    client = WiFiClient();
    WiFiClient newClient = server.available();
    if (newClient) {
      client        = newClient;
      lastActivity  = millis();
      lastKeepAlive = millis();
    }
  }

  if (client && client.connected()) {

    // ── Keepalive ping every 5s ──
    if (millis() - lastKeepAlive > KEEPALIVE_INTERVAL) {
      client.print("$");
      lastKeepAlive = millis();
    }

    // ── Timeout 15s ──
    if (millis() - lastActivity > CLIENT_TIMEOUT) {
      client.stop();
      lastActivity = millis();
      return;
    }
  }

  // ── TX ESP → PIC RX ──
  static String wifiMsg = "";
  static bool waitingEcho = false;
  static unsigned long echoTime = 0;

  // ✅ After sending to PIC, flush echo after 50ms
  if (waitingEcho && millis() - echoTime > 50) {
    waitingEcho = false;
    while(Serial.available()) Serial.read();  // flush COMPIM echo
  }

  // ✅ if() not while() — prevents reading own echo
  if (!waitingEcho && client && client.connected() && client.available()) {
    char c = client.read();
    lastActivity = millis();

    if (c == '$') {
      client.print("$");        // echo keepalive
    } else if (c == '#') {
      wifiMsg = "#";
    } else if (wifiMsg.length() == 1) {
      wifiMsg += c;
      Serial.print(wifiMsg);    // ✅ to PIC only
      blinkLED();
      wifiMsg    = "";
      waitingEcho = true;       // ✅ start echo guard
      echoTime    = millis();
    }
  }

  // ── PIC RX → TX ESP ──
  // ✅ if() not while() + skip during echo guard
  static String picMsg = "";
  if (!waitingEcho && client && client.connected() && Serial.available()) {
    char s = Serial.read();
    lastActivity = millis();

    if (s == '#') {
      picMsg = "#";
    } else if (picMsg.length() == 1) {
      picMsg += s;
      client.print(picMsg);     // ✅ to TX ESP
      blinkLED();
      picMsg = "";
    }
  }

  yield();
}