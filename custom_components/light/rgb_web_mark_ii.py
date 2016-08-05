import logging

# Import the device class from the component that you want to support
from homeassistant.components.light import ATTR_RGB_COLOR, ATTR_TRANSITION, Light
from homeassistant.const import CONF_HOST, CONF_API_KEY, CONF_NAME

# Home Assistant depends on 3rd party packages for API specific code.
REQUIREMENTS = ['https://github.com/corbanmailloux/RGBWebMark2/archive/v0.1.1.zip#rgb_web==0.1.1']
_LOGGER = logging.getLogger(__name__)

DEVICE_DEFAULT_NAME = 'RGB Web Mark II'
DEFAULT_TRANSITION = 0

def setup_platform(hass, config, add_devices, discovery_info=None):
    import rgb_web

    # Validate passed in configuration
    host = config.get(CONF_HOST)
    api_key = config.get(CONF_API_KEY)
    name = config.get(CONF_NAME, DEVICE_DEFAULT_NAME)

    if host is None or api_key is None:
        _LOGGER.error('Invalid configuration. Expected %s and %s',
                      CONF_HOST, CONF_API_KEY)
        return False

    light = rgb_web.RGBWebLight(host, api_key)

    add_devices([RGBWebLight(light, name)])

class RGBWebLight(Light):

    def __init__(self, light, name):
        self._light = light
        self._name = name

    @property
    def name(self):
        """Return the name of the light, if any."""
        return self._name

    @property
    def rgb_color(self):
        """Return the RGB value."""
        return self._light.get_rgb()

    @property
    def is_on(self):
        """Return true if light is on."""
        return self._light.is_on()

    def turn_on(self, **kwargs):
        """Instruct the light to turn on."""
        transition_time = int(kwargs.get(ATTR_TRANSITION, DEFAULT_TRANSITION))

        if ATTR_RGB_COLOR in kwargs:
            rgb_input = kwargs[ATTR_RGB_COLOR]
            self._light.set_rgb(rgb_input[0], rgb_input[1], rgb_input[2], transition_time)
        else:
            self._light.set_rgb(255, 255, 255, transition_time)

    def turn_off(self, **kwargs):
        """Instruct the light to turn off."""
        self._light.turn_off()

    # def update(self):
    #     """Fetch new state data for this light.

    #     This is the only method that should fetch new data for Home Assistant.
    #     """
    #     self._light.update_internal_status()
