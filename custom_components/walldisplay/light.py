# This custom component allows the control of a Philips digital signage display.
# https://www.usa.philips.com/c-p/10BDL3051T_00/signage-solutions-multi-touch-display
# It uses the SICP protocol:
# https://www.keren.nl/dynamic/media/1/documents/Drivers/The%20SICP%20Commands%20Document%20V1_99%2025%20May2017.pdf
#
# This component exposes two new lights:
# - Wall Display Light: The RGB LEDs on the side of the display.
# - Wall Display Screen: The screen itself. This should probably be a switch component, but oh well.

import logging

import voluptuous as vol
import socket  # This should probably be inside something below.

# Import the device class from the component that you want to support
from homeassistant.components.light import (
    SUPPORT_COLOR,
    LightEntity,
    PLATFORM_SCHEMA,
    ATTR_HS_COLOR,
)
from homeassistant.const import CONF_HOST
import homeassistant.helpers.config_validation as cv
import homeassistant.util.color as color_util

_LOGGER = logging.getLogger(__name__)

# Validation of the user's configuration
PLATFORM_SCHEMA = PLATFORM_SCHEMA.extend({vol.Required(CONF_HOST): cv.string})


def setup_platform(hass, config, add_devices, discovery_info=None):

    # Assign configuration variables. The configuration check takes care they are
    # present.
    host = config.get(CONF_HOST)

    helper = WallDisplayHelper(host)

    # Add devices
    add_devices([WallDisplayLight(helper), WallDisplayBacklight(helper)])


class WallDisplayHelper:
    def __init__(self, host):
        self._host = host
        self._port = 5000

    # Turn off the display
    def sleep(self):
        self._send_data(b"\x18\x01")

    # Turn on the display
    def wake(self):
        self._send_data(b"\x18\x02")

    # Set the side LEDs to the given color.
    # Range for each: 0 - 255
    def set_color(self, r, g, b):
        ba = bytearray(b"\xF3\x01")
        ba.append(r)
        ba.append(g)
        ba.append(b)
        self._send_data(ba)

    # Helper to actually send the data.
    def _send_data(self, ba):
        size = len(ba) + 4  # 4 other standard bytes
        ba1 = bytearray()
        ba1.append(size)
        ba1.append(0x01)
        ba1.append(0x00)
        ba1.extend(ba)
        check = 0
        for i in ba1:
            check = check ^ i
        ba1.append(check)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self._host, self._port))
        s.send(ba1)
        # print(s.recv(2048)) # Debug printout
        s.close()


class WallDisplayLight(LightEntity):
    def __init__(self, helper):
        self._helper = helper
        self._name = "Wall Display Light"
        self._state = False
        self._color = [0, 0]  # White

        self._supported_features = SUPPORT_COLOR

    @property
    def name(self):
        """Return the display name of this light."""
        return self._name

    @property
    def is_on(self):
        """Return true if light is on."""
        return self._state

    def turn_on(self, **kwargs):
        """Turn on a LED."""
        if ATTR_HS_COLOR in kwargs:
            self._color = kwargs[ATTR_HS_COLOR]

        self._helper.set_color(*color_util.color_hs_to_RGB(*self._color))

        self._state = True
        self.schedule_update_ha_state()

    def turn_off(self, **kwargs):
        """Instruct the light to turn off."""
        self._helper.set_color(0, 0, 0)
        self._state = False
        self.schedule_update_ha_state()

    @property
    def assumed_state(self):
        """Return true if we do optimistic updates."""
        return True

    @property
    def supported_features(self):
        """Flag supported features."""
        return self._supported_features

    @property
    def should_poll(self):
        return False

    @property
    def hs_color(self):
        """Return the color property."""
        return self._color


class WallDisplayBacklight(LightEntity):
    def __init__(self, helper):
        self._helper = helper
        self._name = "Wall Display Screen"
        self._state = True  # Default to on

    @property
    def name(self):
        """Return the display name of this light."""
        return self._name

    @property
    def is_on(self):
        """Return true if light is on."""
        return self._state

    def turn_on(self, **kwargs):
        self._helper.wake()

        self._state = True
        self.schedule_update_ha_state()

    def turn_off(self, **kwargs):
        """Instruct the light to turn off."""
        self._helper.sleep()
        self._state = False
        self.schedule_update_ha_state()

    @property
    def assumed_state(self):
        """Return true if we do optimistic updates."""
        return True

    @property
    def should_poll(self):
        return False
