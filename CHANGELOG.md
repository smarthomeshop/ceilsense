# Changelog

All notable customer-facing firmware changes for CeilSense are tracked in this file.

This changelog starts on 2026-04-15. Earlier firmware versions existed before that date, but they were not tracked in a customer-facing changelog.

## [Unreleased]

- Delayed cloud registration until WiFi is connected and stopped automatic OTA manifest checks during boot, reducing startup watchdog resets and early network errors on cloud firmware.
- Fixed a build failure with ESPHome 2026.4.0 where `play_stream` was incorrectly nested under `text_sensor`, causing validation errors on devices using the LD2450 variant.
- Added SmartHomeShop App cloud firmware variants for CeilSense WiFi and Ethernet builds, including LD2412, LD2450, and Complete SCD4x families.
- Expanded the Home Assistant firmware selector to switch between WiFi local, Ethernet local, WiFi cloud, and Ethernet cloud variants within the same CeilSense firmware family.

## [CeilSense V1 1.28] - 2026-04-15

### Added

- Added a dedicated **Firmware Update** button in Home Assistant for manual firmware installation after choosing the firmware type.
- Added customer-facing release notes in `CHANGELOG.md` and GitHub Releases for future CeilSense firmware versions.

### Fixed

- Switching **Firmware Type** between **WiFi** and **Ethernet** now refreshes OTA metadata before installation, so the selected network variant is the one that gets flashed.

### Changed

- GitHub Actions now validates that firmware changes include a customer-facing changelog entry.
- The firmware publish workflow now creates version tags, GitHub Releases, and release notes from this changelog.
