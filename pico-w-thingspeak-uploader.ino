/*
 * Pico W ThingSpeak Uploader
 * --------------------------
 * Uploads the RP2040's on-chip temperature to a ThingSpeak channel over
 * HTTPS every 20 seconds. No sensor, no wiring: plug in the Pico W and you
 * have a live cloud chart. A great first cloud project.
 *
 * Note: ThingSpeak's free tier accepts an update at most once every 15 s.
 * This sketch uses 20 s to stay comfortably inside that limit.
 *
 * Required libraries (install via Library Manager):
 *   - "ThingSpeak" by MathWorks
 *
 * Board: Raspberry Pi Pico W with the "Raspberry Pi Pico/RP2040/RP2350"
 *        core by Earle F. Philhower, III (arduino-pico). Select
 *        Tools -> Board -> Raspberry Pi Pico W.
 *
 * Wiring: none. The temperature comes from the on-chip sensor (ADC ch. 4).
 *
 * Setup:
 *   1. Create a ThingSpeak channel with Field 1 = Temperature.
 *   2. Copy config.h.example to config.h; fill WiFi + channel ID + write key.
 *   3. Flash. Open Serial Monitor at 115200. Watch the channel chart update.
 *
 * TLS note: this example calls setInsecure() so it works out of the box.
 * That encrypts traffic but does NOT verify the server. For production, load
 * the server's root CA with setTrustAnchors() (see README).
 *
 * (c) 2026 Fluidwire. Released under the MIT License.
 * https://github.com/fluidwire
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#define TS_ENABLE_SSL        // make the ThingSpeak library use port 443
#include <ThingSpeak.h>
#include "config.h"

const uint32_t UPLOAD_INTERVAL_MS = 20000;   // >= 15 s for free tier
const uint32_t WIFI_TIMEOUT_MS    = 20000;

WiFiClientSecure client;
uint32_t         lastUpload = 0;

bool connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  Serial.printf("WiFi: connecting to %s ", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT_MS) {
      Serial.println(" timeout, will retry");
      return false;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.printf(" OK, ip=%s\n", WiFi.localIP().toString().c_str());
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1500);                 // give USB serial time to enumerate
  client.setInsecure();        // see TLS note above
  connectWiFi();
  ThingSpeak.begin(client);
}

void loop() {
  if (!connectWiFi()) {
    delay(2000);
    return;
  }

  uint32_t now = millis();
  if (lastUpload != 0 && now - lastUpload < UPLOAD_INTERVAL_MS) return;
  lastUpload = now ? now : 1;

  float tempC = analogReadTemp();   // arduino-pico: on-chip sensor, deg C
  ThingSpeak.setField(1, tempC);

  int code = ThingSpeak.writeFields(TS_CHANNEL_ID, TS_WRITE_API_KEY);
  if (code == 200) {
    Serial.printf("ThingSpeak OK: chip temp=%.1fC\n", tempC);
  } else {
    Serial.printf("ThingSpeak write failed, code %d\n", code);
  }
}
