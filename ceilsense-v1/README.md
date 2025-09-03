# CeilSense v1

This folder contains eight firmware variants for CeilSense v1 hardware. All variants include the shared `base.yaml` and add either `wifi.yaml` or `eth.yaml`. Some variants add optional sensor packages.

## Variant naming

- **Basic**: Core device features with built‑in sensors from `base.yaml` (BH1750 illuminance, BMP3xx temperature/pressure), status LED, OTA, diagnostics.
- **Complete**: Everything in Basic plus CO₂ monitoring via the SCD4x package.
- **WiFi vs Ethernet**: Chooses the network stack (`wifi.yaml` or `eth.yaml`).
- **LD2412 suffix**: Adds the LD2412 mmWave presence sensor integration. Use these only if the LD2412 module is present.

## Variants

Basic:
- `ceilsense-basic-wifi.yaml` — WiFi only.
- `ceilsense-basic-wifi-ld2412.yaml` — WiFi + LD2412 presence.
- `ceilsense-basic-eth.yaml` — Ethernet only.
- `ceilsense-basic-eth-ld2412.yaml` — Ethernet + LD2412 presence.

Complete (SCD4x required):
- `ceilsense-complete-wifi.yaml` — WiFi + SCD4x.
- `ceilsense-complete-wifi-ld2412.yaml` — WiFi + SCD4x + LD2412 (LD2412 required for this variant).
- `ceilsense-complete-eth.yaml` — Ethernet + SCD4x.
- `ceilsense-complete-eth-ld2412.yaml` — Ethernet + SCD4x + LD2412 (LD2412 required for this variant).

## Quick selection guide

- **I need CO₂ data**: Pick a Complete variant and ensure an SCD4x sensor is connected.
- **I need presence detection**: Pick a variant with the `-ld2412` suffix and install the LD2412 module.
- **Network choice**: Use WiFi variants for wireless setups; use Ethernet variants when you have wired UTP cable and optional PoE for power.

## Notes

- Each variant sets a hardware identifier via `ceilsense_hardware_version` and a firmware update manifest URL. During CI builds, the version is injected into `base.yaml`.
- Over‑the‑air updates: each published variant has its own manifest; see `update_manifest_url` in the YAML for the exact URL.
