# CeilSense for Home Assistant / ESPHome

![CeilSense](images/promo1.jpg)

CeilSense is a ceiling-mounted ESPHome sensor for Home Assistant that combines presence detection, air-quality monitoring, and ambient sensing in one device. It is designed for fully local operation and supports both WiFi and Ethernet firmware variants.

Product page: https://ceilsense.nl/en

## How It Works

CeilSense combines a radar presence sensor with environmental sensors in a ceiling-mounted device. Depending on the selected firmware variant, it connects over WiFi or Ethernet and sends its data directly to Home Assistant through ESPHome.

## Key Features

- Radar-based presence sensing
- CO2, temperature, and humidity monitoring
- Illuminance and pressure sensing
- WiFi and Ethernet variants, including PoE-capable hardware
- Captive portal and Improv provisioning on WiFi builds
- Fully local operation with OTA updates through published manifests
- Manual switching between WiFi and Ethernet firmware families

## Hardware Versions

| Version | Chip | Connectivity | Description |
|---------|------|--------------|-------------|
| V1 Basic | ESP32-S3 | WiFi and Ethernet | Presence and ambient sensing platform without SCD4x CO2 module |
| V1 Complete | ESP32-S3 | WiFi and Ethernet | Full sensor platform with SCD4x CO2 support |

## Variants

We publish customer-facing variants for Basic and Complete hardware, with WiFi or Ethernet networking and optional LD2412 or LD2450 radar support.

| Hardware | Variant | Description |
|----------|---------|-------------|
| V1 Basic (ESP32-S3) | WiFi | WiFi build without external radar package |
| V1 Basic (ESP32-S3) | WiFi + LD2412 | WiFi build with LD2412 presence radar |
| V1 Basic (ESP32-S3) | WiFi + LD2450 | WiFi build with LD2450 multi-target radar |
| V1 Basic (ESP32-S3) | Ethernet | Ethernet build without external radar package |
| V1 Basic (ESP32-S3) | Ethernet + LD2412 | Ethernet build with LD2412 presence radar |
| V1 Basic (ESP32-S3) | Ethernet + LD2450 | Ethernet build with LD2450 multi-target radar |
| V1 Complete (ESP32-S3) | WiFi | WiFi build with SCD4x CO2 support |
| V1 Complete (ESP32-S3) | WiFi + LD2412 | WiFi build with SCD4x and LD2412 presence radar |
| V1 Complete (ESP32-S3) | WiFi + LD2450 | WiFi build with SCD4x and LD2450 multi-target radar |
| V1 Complete (ESP32-S3) | Ethernet | Ethernet build with SCD4x CO2 support |
| V1 Complete (ESP32-S3) | Ethernet + LD2412 | Ethernet build with SCD4x and LD2412 presence radar |
| V1 Complete (ESP32-S3) | Ethernet + LD2450 | Ethernet build with SCD4x and LD2450 multi-target radar |

## Getting Started

1. Install the device in the ceiling and connect power.
2. Flash the desired firmware with ESP Web Tools or ESPHome CLI.
3. Use Improv or captive portal for WiFi onboarding where applicable.
4. Ethernet builds connect automatically and wait for API access.
5. To switch later, change `Firmware Type` in Home Assistant and run the firmware update entity.

Quick start guide: https://smarthomeshop.io/quick-start-ceilsense

## Version History

- Customer-facing release notes: [CHANGELOG.md](CHANGELOG.md)
- GitHub Releases: https://github.com/smarthomeshop/ceilsense/releases

## Repository Layout

- `ceilsense-v1/` — ESPHome configurations, packages, and customer-facing firmware variants
- `.github/workflows/` — build, validation, manifest publishing, and release automation
- `CHANGELOG.md` — customer-facing firmware release notes
- `gh-pages` branch — public firmware files and manifests

## Gallery

| Top view | In-ceiling |
| --- | --- |
| ![Top view](images/ceilsense-topview1.png) | ![In ceiling](images/ceilsense-in-ceiling-cutout.jpg) |

| Bottom view |
| --- |
| ![Bottom view](images/ceilsense-bottomview.png) |

## Contributing

PRs and issues are welcome. Please keep changes modular and follow ESPHome best practices.

## Support

- Product info and guides: https://ceilsense.nl/en
- Store: https://smarthomeshop.io
- Community and support: https://smarthomeshop.io/discord

## License

This project is released under the CC BY-NC 4.0 license.
