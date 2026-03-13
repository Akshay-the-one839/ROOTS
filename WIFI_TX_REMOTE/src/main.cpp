#include <Arduino.h>
#include <WiFi.h>

const char* ssid     = "ESP_RX";
const char* password = "11111111";
IPAddress serverIP(192, 168, 4, 1);
WiFiClient client;

#define LED_PIN 2
unsigned long blinkTime     = 0;
bool ledOn                  = false;

unsigned long lastKeepAlive = 0;
const unsigned long KEEPALIVE_INTERVAL = 5000;

HardwareSerial picSerial(1);   // UART1: TX=43, RX=44

void blinkLED() { digitalWrite(LED_PIN, LOW); blinkTime = millis(); ledOn = true; }

void setup() {
  Serial.begin(9600);          // debug only
  picSerial.begin(9600, SERIAL_8N1, 44, 43);  // RX=44, TX=43
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Wait for PIC boot messages to finish, then flush
  delay(2000);
  while(picSerial.available()) picSerial.read();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {

  if (ledOn && millis() - blinkTime > 30) {
    digitalWrite(LED_PIN, HIGH);
    ledOn = false;
  }

  // ── Reconnect if dropped ──
  if (!client.connected()) {
    client.stop();
    if (client.connect(serverIP, 80)) {
      // connected
    } else {
      delay(1000);
      return;
    }
  }

  // ── Keepalive ──
  if (millis() - lastKeepAlive > KEEPALIVE_INTERVAL) {
    client.print("$");
    lastKeepAlive = millis();
  }

  // ── PIC TX → RX ESP ──
  static String picMsg = "";
  static bool waitingEcho = false;
  static unsigned long echoTime = 0;

  // After sending to PIC, flush echo after 50ms
  if (waitingEcho && millis() - echoTime > 50) {
    waitingEcho = false;
    while(picSerial.available()) picSerial.read();  // flush COMPIM echo
  }

  // if() not while() — one byte per loop
  if (!waitingEcho && picSerial.available()) {
    char c = picSerial.read();
    if (c == '#') {
      picMsg = "#";
    } else if (picMsg.length() == 1) {
      picMsg += c;
      client.print(picMsg);     // to RX ESP via WiFi
      blinkLED();
      picMsg = "";
    }
  }

  // ── RX ESP → PIC TX ──
  static String wifiMsg = "";

  // if() not while() + skip during echo guard
  if (!waitingEcho && client.available()) {
    char c = client.read();
    if (c == '$') {
      client.print("$");        // echo keepalive
    } else if (c == '#') {
      wifiMsg = "#";
    } else if (wifiMsg.length() == 1) {
      wifiMsg += c;
      picSerial.print(wifiMsg);    // to PIC only
      blinkLED();
      wifiMsg     = "";
      waitingEcho = true;          // start echo guard
      echoTime    = millis();
    }
  }

  yield();
}