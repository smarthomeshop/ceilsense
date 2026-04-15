# Changelog

All notable customer-facing firmware changes for CeilSense are tracked in this file.

This changelog starts on 2026-04-15. Earlier firmware versions existed before that date, but they were not tracked in a customer-facing changelog.

## [Unreleased]

- Add customer-facing firmware notes here before merging a PR.

## [CeilSense V1 1.28] - 2026-04-15

### Added

- Added a dedicated **Firmware Update** button in Home Assistant for manual firmware installation after choosing the firmware type.
- Added customer-facing release notes in `CHANGELOG.md` and GitHub Releases for future CeilSense firmware versions.

### Fixed

- Switching **Firmware Type** between **WiFi** and **Ethernet** now refreshes OTA metadata before installation, so the selected network variant is the one that gets flashed.

### Changed

- GitHub Actions now validates that firmware changes include a customer-facing changelog entry.
- The firmware publish workflow now creates version tags, GitHub Releases, and release notes from this changelog.
