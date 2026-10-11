# PICO-W-THINGSPEAK-UPLOADER — upload the on-chip temperature to ThingSpeak

IoT example by [Fluidwire](https://fluidwire.com/).

Uploads the Raspberry Pi Pico W's built-in temperature reading to a ThingSpeak channel every 20 seconds over HTTPS. There is nothing to wire: plug in the board and you have a live cloud chart, which makes this a zero-hardware first cloud project for the Pico W.

## What it does

- Connects to WiFi using credentials in `config.h` and reconnects if the link drops
- Every 20 s, reads the RP2040 on-chip temperature sensor (ADC channel 4) with `analogReadTemp()`
- Writes the value to Field 1 of your ThingSpeak channel over HTTPS (port 443)

ThingSpeak's free tier accepts an update at most once every 15 seconds; the 20 s interval stays safely inside that. The on-chip sensor reads the chip's die temperature, so expect it to run a few degrees above room temperature.

## Required hardware

- 1x Raspberry Pi Pico W
- 1x micro-USB cable

## Required libraries

Install via the Arduino IDE Library Manager (**Sketch → Include Library → Manage Libraries…**):

- **ThingSpeak** by MathWorks

You also need the Pico core via **Boards Manager**: search for `Raspberry Pi Pico/RP2040/RP2350` by Earle F. Philhower, III (arduino-pico), then select **Tools → Board → Raspberry Pi Pico W**.

## Wiring

None. The temperature sensor is inside the RP2040.

## Setup

1. At [thingspeak.com](https://thingspeak.com), create a free account and a new channel with **Field 1** (e.g. "Chip Temperature").
2. Note the **Channel ID** and the **Write API Key** (channel's *API Keys* tab).
3. Copy `config.h.example` to `config.h` and fill in WiFi + channel ID + write key.
4. Select the Pico W board and port, then **Upload**.

## Expected output

Open **Tools → Serial Monitor**, baud rate **115200**:

```
WiFi: connecting to your-wifi-ssid .... OK, ip=192.168.1.62
ThingSpeak OK: chip temp=28.4C
ThingSpeak OK: chip temp=28.4C
```

Open your channel on thingspeak.com — the Field 1 chart fills in as readings arrive.

## Troubleshooting

- **`ThingSpeak write failed, code 0`** — no network or TLS connection. Confirm WiFi connected and got an IP.
- **Code `-301` or repeated failures** — updates sent faster than every 15 s, or a wrong write key. Keep the interval ≥ 15 s and re-check the key.
- **Code 200 but the chart is empty** — the channel has no Field 1 enabled. Enable it in the channel settings.
- **Nothing in the Serial Monitor** — the sketch waits 1.5 s for USB serial; reopen the monitor or press reset.
- **WiFi never connects** — wrong SSID/password or a 5 GHz-only network. The Pico W is 2.4 GHz only.

## Security note

Don't commit `config.h` — it holds your WiFi password and ThingSpeak write key. The `.gitignore` already excludes it; only `config.h.example` is tracked. Treat the write key like a password; you can regenerate it on ThingSpeak if it leaks.

The sketch calls `client.setInsecure()` so it works without extra setup. Traffic is encrypted but the server certificate is not verified. For production, fetch the root CA for `api.thingspeak.com` and pass it to `client.setTrustAnchors()`.

## License

MIT — see [LICENSE](LICENSE).

## Got stuck?

Drop by [fluidwire.com](https://fluidwire.com/) — we ship IoT building blocks and are happy to help.
