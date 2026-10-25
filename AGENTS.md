# AGENTS.md

This is the **live configuration of a running Home Assistant instance** (Home Assistant OS on a HA Blue / ODROID-N2+, version in `.HA_VERSION`), mounted at `/Volumes/config` and pushed to `github.com/corbanmailloux/home-assistant-configuration`. There is no build, test suite, or CI — "verify" means HA validates the config (no local `hass` binary is available from this mount).

## Live instance access (MCP)

- A `home-assistant` MCP server (custom component `ha_mcp_tools`, installed via HACS; artifacts in `.ha_mcp/`) exposes the **running instance**: query entities/states, call services, reload YAML, restart. Use it to check real entity IDs and states instead of guessing from YAML.
- Prefer reload/check tools over `ha_restart` — a restart takes the whole house's automations down for minutes.
- The server ships a best-practices skill (`skill://home-assistant-best-practices/SKILL.md`); consult it before writing automations, scripts, dashboards, or helpers.

## Critical context

- This is **production**: garage doors (ratgdo), Lutron Caseta, HVAC, curtains. YAML edits take effect on the physical house on reload. Never call actuating services "just to test" without checking what they control.
- `git status` is often dirty: packages are edited live from mobile and dashboards change via UI. Don't assume a clean tree; don't revert or stash unrelated changes. Don't commit or push unless explicitly requested.

## Structure

- `configuration.yaml` is a minimal hub. Nearly all config lives in **`packages/`** (`!include_dir_named packages`): every `.yaml` file there auto-loads as a package named by its filename. Add new config as a new package file (top level = integration domain keys only, e.g. `template:`, `automation:`); package/file names must be unique.
- `automations.yaml`, `scenes.yaml`, `scripts.yaml` are owned by the UI editors. Dashboards live in `.storage/lovelace*` — these `.storage` files **are** tracked (the rest of `.storage/` is gitignored), so UI dashboard edits show in `git status`.
- Preserve `id:` / `unique_id:` UUIDs and `device_id:` references in device triggers — they link YAML to the UI registries; regenerating them orphans UI entries.
- Automations use the modern schema: `triggers:` / `conditions:` / `actions:` (not legacy `trigger:` / `action:`).
- `group/`, `light/`, `notify/` are small YAML-mode includes merged in `configuration.yaml`.

## Subsystems & secrets

- `esphome/` — ESPHome device configs for the ESPHome add-on, with its **own `secrets.yaml`** and shared bases in `common/`. VS Code maps `**/esphome/*.yaml` to the ESPHome schema and all other YAML to the HA schema (`.vscode/settings.json`) — keep each format in its own directory.
- `custom_components/` and themes `midnight`, `mushroom`, `noctis`, `ios-themes` are HACS-managed and gitignored — never hand-edit or commit them.
- Root `secrets.yaml`, `esphome/secrets.yaml`, `google_cloud_credentials.json`, and `*.pem` are local-only. Reference values with `!secret <key>`; never commit credentials.
- Errors from `octoprint` / `moonraker` in `home-assistant.log.*` are expected noise (3D printers are often offline; see `logger:` in `configuration.yaml`).

## Validation

- No CI. Validate with **Developer Tools → YAML → Check configuration** in the HA UI, or via the MCP server, before considering a change done.
- For YAML-only changes (automations, scripts, templates), reload the specific config instead of restarting; adding a new integration or package entry requires a full restart.
- Work lands on `master` directly; occasional feature branches exist for large projects (e.g. `unicycle-monitor`).
