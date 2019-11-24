import appdaemon.plugins.hass.hassapi as hass

# App to respond to presses of individual WiFi buttons.
#
# Args:
# event_name - Name of the event to listen for.

# Valid types of button presses.
PRESS_TYPES = ["single", "double", "hold"]


class BedsideButton(hass.Hass):
    def initialize(self):
        self.listen_event(self.press_event, self.args["event_name"])

    def press_event(self, event_name, data, kwargs):
        self.log("Event: {}, data = {}, args = {}".format(event_name, data, kwargs))

        # Argument validation
        if "press_type" not in data:
            self.log("press_type missing from event data. Skipping.")
            return

        press_type = data["press_type"]
        if press_type not in PRESS_TYPES:
            self.log(
                "Invalid press_type '{}' in event data. Skipping.".format(press_type)
            )
            return

        if press_type == "single":
            self.toggle("light.fairy_lights")
        elif press_type == "double":
            self.toggle("switch.bedroom_fan")
        elif press_type == "hold":
            self.turn_on("script.everything_off")
