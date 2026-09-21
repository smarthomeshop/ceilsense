# Web installer: manual CeilSense LED test

This protocol supplements `SHS_TEST/1` automatic hardware-test logs. It does not
replace the `begin`, `test` or `result` messages. It is supported by the modular
CeilSense V1 Wi-Fi/Ethernet variants, local and cloud, after firmware containing
`shs_test_serial` is installed. Older firmware, including V1 1.39, and the legacy
`single` configurations do not support it. Discover support; do not infer it
from the product name or firmware version alone.

## Connection and discovery

Use the existing USB serial connection at **115200 baud, 8N1**, after flashing and
rebooting. Do not open a second serial connection, compete with another reader,
or toggle DTR/RTS just to send a command. Keep the hardware-test log reader open
after its automatic `result`; manual LED tests are independent and repeatable.

Send UTF-8/ASCII bytes terminated with LF (`\n`); CRLF is also accepted:

```text
SHS_TEST/1 capabilities discover-1
```

Every command needs a request ID of 1–32 ASCII letters, digits, `_` or `-`.
The response is JSON within an ordinary ESPHome log line, which can contain a
timestamp, tag and ANSI color escapes before/after it:

```json
{"type":"capabilities","request_id":"discover-1","commands":["led_test"],"id":"status_led","effect":"rainbow","duration_ms":8000,"requires_visual_confirmation":true}
```

The JSON follows `SHS_TEST/1 `. Buffer partial serial reads until a whole log line
is received, strip ANSI escapes, find the marker, and parse JSON from there.
Ignore unrelated logs and unknown message types. Capabilities are also announced
at startup with an empty request ID; actively querying avoids missed boot logs.
Allow 3 seconds per discovery request and retry at most twice while connected.
Without a response advertising `led_test`, retain the existing manual check and
explain that this firmware does not offer the USB LED test. Never report a
hardware failure merely because an old firmware does not answer.

The firmware has one physical UART reader, forwarding complete Improv frames to
ESPHome's normal Improv handler. On the website, serialize writes with existing
Improv operations and preserve its binary parser; do not discard or decode all
binary traffic as JSON. No Wi-Fi, cloud account, API encryption key, or saved
startup-LED setting is required for a manual test.

## Run a test

For an explicit click on **Voer test uit**, generate a fresh request ID and send:

```text
SHS_TEST/1 led_test led-42
```

Firmware replies with the following `SHS_TEST/1` JSON (shown without log prefix):

```json
{"type":"manual_test","id":"status_led","request_id":"led-42","status":"running","reason":"","duration_ms":8000,"requires_visual_confirmation":true}
```

The ring runs **Hardware Test Rainbow** for eight seconds at 65% brightness.
Normal LED automation pauses while it runs. The previous on/off state, color,
brightness and effect are restored afterward. No persistent switch or light
preference is changed. Even if the browser disconnects, firmware stops the test
on its own deadline. Completion uses the same event and request ID, with
`"status":"complete"`.

Do not display a running countdown until `running` has been acknowledged. Use
the reported duration. Allow 3 seconds for acknowledgement and 12 seconds from
acknowledgement for completion. The website must handle missing responses,
disconnects, a reboot, and stale events from older requests without marking the
test successful. A retry using the same request ID repeats its status without
restarting the timer; only the most recently completed ID is remembered until
reboot. Use a new ID when the user intentionally repeats a test.

Other responses have the same `manual_test` structure:

| Status | Reason | Meaning / UI |
| --- | --- | --- |
| `busy` | `test_in_progress` | Another LED test owns the ring. Let the user retry later. |
| `busy` | `led_automation_active` | An opt-in boot self-test or CO₂ flash is active. Retry later. |
| `error` | `effect_unavailable` | Firmware cannot start the configured effect. Show an error. |

Malformed, oversized and unsupported commands are ignored. They never enable
LEDs or change saved settings. Do not send arbitrary user text as a command.

## Human result and UI

Replace the static manual text in the **Status LED ring** row with a button only
when the capability is advertised. Use `Voer test uit`, then `Test wordt gestart…`,
then `Regenboogtest bezig…`. Disable repeated clicks while starting/running.
After completion ask **Zag je de ledring in regenboogkleuren draaien?** with
**Ja, werkt** and **Nee, werkt niet**, plus **Opnieuw testen**.

`complete` means the animation finished, **not** that the LEDs were seen working.
The visual pass/fail belongs to the user. Keep it distinct from the automatic
sensor-test result; a delayed automatic `test` event with `status: skip` for
`status_led` must not erase a running test or a visual result. Likewise, a global
automatic `result: pass` must not mark the LED test passed. A failed/cancelled
serial command is a connection/test-execution problem, not proof of bad LEDs.

Preserve all existing P1MeterKit and other product flows. Add tests for split log
lines, ANSI prefixes, old firmware, discovery timeout, duplicate clicks, retries,
busy/error responses, disconnects, stale request IDs, automatic results arriving
during a manual test, and explicit visual pass/fail confirmation.

## Firmware development

Requires ESPHome 2026.9.0 or later for the public Improv UART transport hook.
The default component source supports GitHub dashboard imports. CI overrides
the source to the current checkout so release builds never use stale component
code from a remote cache. Local example from the repository root:

```sh
esphome -s shs_test_component_source ../components config ceilsense-v1/ceilsense-complete-wifi-ld2412-factory-hardware-test.yaml
python -m unittest discover -s tests -v
```
