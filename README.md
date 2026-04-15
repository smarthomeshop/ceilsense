# CeilSense for Home Assistant / ESPHome

![CeilSense](images/promo1.jpg)

CeilSense is a ceiling-mounted, all-in-one room sensor designed to deliver reliable presence, air-quality and ambient measurements. It integrates seamlessly with Home Assistant via ESPHome and runs fully local (no cloud required).

Learn more on our website: https://ceilsense.nl/en

## Key features

- **Presence sensing (radar)**: Accurate motion/still detection with default included LD2412 mmwave module.
- **Air quality**: CO₂, temperature, humidity (SCD41).
- **Ambient**: Illuminance (BH1750), pressure/altitude (BMP3xx).
- **Connectivity**: WiFi or Ethernet with included PoE variants.
- **Provisioning**: Improv over BLE/Serial and captive portal (WiFi builds).
- **Local only**: Works without cloud services; OTA supported via manifest on GitHub Pages.
- **Manual OTA switching**: Choose **Firmware Type** in Home Assistant, then press **Firmware Update** to install the selected WiFi or Ethernet variant.

## Variants

We publish multiple firmware variants for Basic and Complete hardware, including WiFi and Ethernet builds plus LD2412/LD2450 options. Each customer-facing variant has its own YAML in `ceilsense-v1/` and matching Web Tools manifest on the `gh-pages` branch.

## Getting started

1. **Hardware**: Install the device in the ceiling and connect power (and Ethernet if applicable).
2. **Flash firmware**:
   - Use ESP Web Tools from our docs or GitHub Pages (per‑variant manifest).
   - Or compile/flash locally with ESPHome CLI.
3. **Onboarding**:
   - WiFi builds support Improv (BLE/Serial) and captive portal.
   - Ethernet builds connect automatically and wait for API.
4. **Switch firmware later**:
   - In Home Assistant, change **Firmware Type** to `WiFi` or `Ethernet`.
   - Press **Firmware Update** to install the selected firmware family.

Please check for full documentation our quick start guide: https://smarthomeshop.io/quick-start-ceilsense

## Repository layout

- `ceilsense-v1/` — ESPHome configurations (base, mixins, packages, and customer-facing firmware variants)
- `.github/workflows/` — CI to build firmware, validate changelog updates, publish artifacts/manifests to `gh-pages`, and create GitHub Releases
- `CHANGELOG.md` — customer-facing firmware release notes used for GitHub Releases
- `gh-pages` branch — public firmware and manifests (for OTA and ESP Web Tools)

## Contributing

PRs and issues are welcome. Please keep changes modular and follow ESPHome best practices.

## Support

- Product info and guides: https://ceilsense.nl/en
- Store: https://smarthomeshop.io
- Community & support (Discord): https://smarthomeshop.io/discord

## License

This project is released under the CC BY-NC 4.0 license (see `license`).

## Gallery

| Top view | In‑ceiling (cut‑out) |
| --- | --- |
| ![Top view](images/ceilsense-topview1.png) | ![In ceiling](images/ceilsense-in-ceiling-cutout.jpg) |

| Bottom view |
| --- |
| ![Bottom view](images/ceilsense-bottomview.png) |
