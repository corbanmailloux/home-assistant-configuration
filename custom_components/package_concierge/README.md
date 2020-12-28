# Package Concierge Scraper

This is a simple scraper that can login and get a count (maximum 5) of packages waiting to be picked up from the Package Concierge package locker system.

It exposes a sensor (by default `sensor.package_concierge_USERNAME`, but overridden by setting `name:`) that is a count of the number of packages in the "Delivered" status.

### Installation

Copy this folder to `<config_dir>/custom_components/package_concierge/`.

Add the following to your `configuration.yaml` file:

```yaml
# Example configuration.yaml entry
sensor:
  - platform: package_concierge
    name: package_concierge_corban # Optional
    username: "USERNAME"
    password: "0000" # PIN
```
