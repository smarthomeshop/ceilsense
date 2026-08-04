# Changelog

All notable customer-facing firmware changes for CeilSense are tracked in this file.

This changelog starts on 2026-04-15. Earlier firmware versions existed before that date, but they were not tracked in a customer-facing changelog.

## [Unreleased]

- Add customer-facing firmware notes here before merging a PR.

## [CeilSense V1 1.38] - 2026-08-04


- Added guided post-install hardware checks when flashing supported CeilSense
  variants through the SmartHomeShop website, for both local and cloud firmware.


## [CeilSense V1 1.37] - 2026-08-01


- Pinned radar mounting metadata to stable release `v1.0.0`; static Room
  Builder profile entities now publish once at startup without polling.


## [CeilSense V1 1.36] - 2026-08-01

- Moved ceiling mounting, coordinate projection and radar metadata to the
  universal SmartHomeShop radar-mounting profiles. Future ceiling products can
  use the same contract with LD2450, LD2460 or LD6002B hardware.


## [CeilSense V1 1.35] - 2026-07-31


- Added explicit ceiling-mount metadata for LD2450 variants so Room Designer
  and other integrations can identify the mounting model without relying only
  on the CeilSense device name.



## [CeilSense V1 1.34] - 2026-07-26


- Automatic LED lighting is now disabled by default for new and updated CeilSense firmware. Motion Light, Night Light, and CO2 LED alerts must be enabled explicitly; saved user settings are preserved.


## [CeilSense V1 1.33] - 2026-07-25


- Added the LED lighting feature to all CeilSense firmware families. Variants with a presence radar now provide Motion Light and Night Light controls, while Complete variants also provide CO2 warning and critical alerts.
- Non-cloud variants now keep the same LED priority and behavior locally through ESPHome or Home Assistant; cloud variants continue to receive the configuration from SmartHomeShop Cloud.
- Fixed the local LED package for current ESPHome builds so all non-cloud variants compile successfully.


## [CeilSense V1 1.32] - 2026-07-24


- Added cloud-controlled CeilSense LED settings for motion light, night light and CO2 warnings. Cloud firmware now applies versioned device configuration, reports whether it was accepted and uses moving-target detection for the motion light while still presence can activate the night light.

- Fixed cloud firmware restarting every 15 minutes when Home Assistant is not connected: the no-client reboot of the native API is now disabled on cloud firmware, so cloud-only setups run uninterrupted.

- Added the branded SmartHomeShop setup portal to all CeilSense WiFi firmware variants: connecting to the fallback hotspot now opens a SmartHomeShop setup page to pick your WiFi network, choose the firmware variant and see next steps for Home Assistant or SmartHomeShop Cloud.
- The setup portal can also switch a CeilSense to the Ethernet firmware: pick Ethernet during setup and the device installs the matching Ethernet variant over WiFi in the background (about two minutes), after which you plug in the network cable. The radar configuration (LD2412 or LD2450) is preserved automatically.

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
