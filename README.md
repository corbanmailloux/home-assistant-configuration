# Corban Mailloux's Home Assistant Configuration

This is the configuration I use for [Home Assistant](https://www.home-assistant.io/). I use [Hass.io](https://www.home-assistant.io/hassio/) running in Docker on an [Atomic Pi](https://dlidirect.com/products/atomic-pi) as the central Home Assistant server.

## Services and Devices

Here are (some of) the services/platforms/other devices that I use with Home Assistant:

Name                                                    | Product Link                                                                                                                                                                                                                                          | How It's Used in HASS
------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TCL Roku TV (Model: 65S405)                             | [TCL](https://www.tclusa.com/products/home-theater/4-series/tcl-65-class-4-series-4k-uhd-led-roku-smart-tv-65s405)                                                                                                                                    | [Roku media player component](https://www.home-assistant.io/components/media_player.roku/)
Roku Streaming Stick                                    | [Amazon](https://www.amazon.com/gp/product/B075XN5L53/)                                                                                                                                                                                               | [Roku media player component](https://www.home-assistant.io/components/media_player.roku/)
ESP8266 (in many ways)                                  | [Generic ESP8266-01](https://www.amazon.com/gp/product/B00PA3UQNI/), [NodeMCU](https://www.amazon.com/gp/product/B010O1G1ES/), [D1 Mini](https://www.amazon.com/gp/product/B01N3P763C/)                                                               | Mostly using [ESPHome](https://esphome.io/), but also some MQTT with [ESPEasy](https://www.letscontrolit.com/wiki/index.php/ESPEasy), [ESP MQTT RGB LED](https://github.com/corbanmailloux/esp-mqtt-rgb-led), [ESP MQTT DHT](https://github.com/corbanmailloux/esp-mqtt-dht), and other custom firmware
Sonoff                                                  | [ITEAD](https://www.itead.cc/sonoff-wifi-wireless-switch.html)                                                                                                                                                                                        | [Sonoff-Tasmota](https://github.com/arendst/Sonoff-Tasmota) and MQTT
Google Home Mini                                        | [Google Store](https://store.google.com/us/product/google_home_mini)                                                                                                                                                                                  | [Google Assistant component](https://www.home-assistant.io/components/google_assistant/)
IKEA Trådfri (Lights, Outlets, and Control Devices)     | IKEA: [Light](https://www.ikea.com/us/en/p/tradfri-remote-control-kit-white-spectrum-50460042/), [Outlet](https://www.ikea.com/us/en/p/tradfri-control-outlet-kit-70364803/), [Remote](https://www.ikea.com/us/en/p/tradfri-remote-control-00443130/) | [Zigbee2MQTT](https://www.zigbee2mqtt.io/) as a Hass.io add-on
OctoPrint with a Monoprice MP Select Mini 3D Printer V2 | [OctoPrint](https://octoprint.org/), [Monoprice](https://www.monoprice.com/product?p_id=21711)                                                                                                                                                        | [OctoPrint component](https://www.home-assistant.io/components/octoprint/)
TP-Link Smart Plug                                      | [Amazon](https://www.amazon.com/gp/product/B01K1JVZOE/)                                                                                                                                                                                               | [TP-Link Switch](https://www.home-assistant.io/components/switch.tplink/)
Roborock S5 Vacuum                                      | [Amazon](https://www.amazon.com/gp/product/B0792BWMV4/)                                                                                                                                                                                               | [Xiaomi Mi Robot Vacuum](https://www.home-assistant.io/integrations/vacuum.xiaomi_miio/)
Aquara (Zigbee) Buttons and Sensors                     | Aliexpress: [Button](https://www.aliexpress.com/item/32998319647.html), [Door Sensor](https://www.aliexpress.com/item/32967550225.html)                                                                                                               | [Zigbee2MQTT](https://www.zigbee2mqtt.io/) as a Hass.io add-on
Windows 10 (Gaming PC)                                  |                                                                                                                                                                                                                                                       | [IOT Link](https://iotlink.gitlab.io/) through MQTT for power/state management
Synology DS1019+ (NAS, Plex Media Server)               | [Amazon](https://www.amazon.com/Synology-Bay-DiskStation-DS1019-Diskless/dp/B07NF9XDWG/)                                                                                                                                                              | [Plex integration](https://www.home-assistant.io/integrations/plex/) for the media server, [NZBGet integration](https://www.home-assistant.io/integrations/nzbget/) for download controls

## Things that Might be Helpful Again

### List all `entity_id`s

During the update to v0.85 of Home Assistant, `slugify` changed. I was nervous this would cause subtle changes in my `entity_id` values and I wouldn't notice, so I exported them all before and after the update. I could have also grabbed this from the "States" page in HASS, but I wanted it as a template.

```
{% for state in states -%}
  {{ state.entity_id }}
{% endfor %}
```

### "Flicker" a traditional light

```yaml
scary_mode_flicker_lights:
  sequence:
    # Delay for 0 to 6 seconds
    - delay: 00:00:{{ (range(0, 7) | random | string).rjust(2, '0') }}
    - service: homeassistant.turn_on
      entity_id: light.entry_lamp
    - delay:
        milliseconds: 100
    - service: homeassistant.turn_off
      entity_id: light.entry_lamp
    - service: script.turn_on
      entity_id: script.scary_mode_flicker_lights_loop

scary_mode_flicker_lights_loop:
  sequence:
    - delay: 00:00:01
    - service: script.turn_on
      entity_id: script.scary_mode_flicker_lights
```

### Extract the battery level from a Google Maps tracker and build a battery icon

```yaml
corban_phone_battery:
  friendly_name: "Corban's Phone Battery"
  device_class: battery
  unit_of_measurement: "%"
  entity_id: "device_tracker.google_maps_110168280884137709870"
  icon_template: >-
    {%- set tracker_name = 'device_tracker.google_maps_110168280884137709870' -%}

    {%- set battery_level = state_attr(tracker_name, 'battery_level') -%}
    {%- set battery_charging = state_attr(tracker_name, 'battery_charging') -%}

    {%- if battery_level is none -%}
      mdi:battery-unknown
    {%- else -%}
      {%- set icon_suffix = ['-outline', '-10', '-20', '-30', '-40', '-50', '-60', '-70', '-80', '-90', ''] -%}
      {%- set charge = (battery_level | float / 10) | round -%}
      mdi:battery{%- if battery_charging -%}-charging{%- endif -%}{{ icon_suffix[charge] }}
    {%- endif -%}
  value_template: >-
    {{ state_attr('device_tracker.google_maps_110168280884137709870', 'battery_level') }}
```
