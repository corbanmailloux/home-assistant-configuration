## Corban Mailloux's Home Assistant Configuration

This is the configuration I use for [Home Assistant](https://www.home-assistant.io/). I use [Hass.io](https://www.home-assistant.io/hassio/) running on a Raspberry Pi as the central Home Assistant server.

### Services and Devices
Here are (some of) the services/platforms/other devices that I use with Home Assistant:

| Name | Product Link | How It's Used in HASS |
|------|--------------|-----------|
| TCL Roku TV (Model: 65S405) | [TCL](https://www.tclusa.com/products/home-theater/4-series/tcl-65-class-4-series-4k-uhd-led-roku-smart-tv-65s405) | [Roku media player component](https://www.home-assistant.io/components/media_player.roku/) |
| ESP8266 (in many ways) | [Generic ESP8266-01](https://www.amazon.com/gp/product/B00PA3UQNI/), [NodeMCU](https://www.amazon.com/gp/product/B010O1G1ES/), [D1 Mini](https://www.amazon.com/gp/product/B01N3P763C/) | MQTT with [ESPEasy](https://www.letscontrolit.com/wiki/index.php/ESPEasy), [ESP MQTT RGB LED](https://github.com/corbanmailloux/esp-mqtt-rgb-led), [ESP MQTT DHT](https://github.com/corbanmailloux/esp-mqtt-dht), and other custom firmware |
| Sonoff | [ITEAD](https://www.itead.cc/sonoff-wifi-wireless-switch.html) | [Sonoff-Tasmota](https://github.com/arendst/Sonoff-Tasmota) and MQTT |
| Google Home Mini | [Google Store](https://store.google.com/us/product/google_home_mini) | [Google Assistant component](https://www.home-assistant.io/components/google_assistant/) |
| IKEA Trådfri | [IKEA](https://www.ikea.com/us/en/catalog/products/90353361/) | [Tradfri component](https://www.home-assistant.io/components/tradfri/) |
| Nest Thermostat E | [Nest Store](https://store.nest.com/product/thermostat-e/T4000ES) | [Nest component](https://www.home-assistant.io/components/nest/) |
| OctoPrint with a Monoprice MP Select Mini 3D Printer V2 | [OctoPrint](https://octoprint.org/), [Monoprice](https://www.monoprice.com/product?p_id=21711) | [OctoPrint component](https://www.home-assistant.io/components/octoprint/) |
| TP-Link Smart Plug | [Amazon](https://www.amazon.com/gp/product/B01K1JVZOE/) | [TP-Link Switch](https://www.home-assistant.io/components/switch.tplink/) |


### Helper Templates

#### List all `entity_id`s
During the update to v0.85 of Home Assistant, `slugify` changed. I was nervous this would cause subtle changes in my `entity_id` values and I wouldn't notice, so I exported them all before and after the update.
I could have also grabbed this from the "States" page in HASS, but I wanted it as a template.
```
{% for state in states -%}
  {{ state.entity_id }}
{% endfor %}
```
