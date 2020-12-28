"""Platform for sensor integration of Package Concierge."""

import logging

import voluptuous as vol
import homeassistant.helpers.config_validation as cv
from homeassistant.components.sensor import PLATFORM_SCHEMA

from homeassistant.const import CONF_PASSWORD, CONF_USERNAME, CONF_NAME
from homeassistant.helpers.entity import Entity
from datetime import timedelta

_LOGGER = logging.getLogger(__name__)

# Validation of the user's configuration
PLATFORM_SCHEMA = PLATFORM_SCHEMA.extend(
    {
        vol.Optional(CONF_NAME): cv.string,
        vol.Required(CONF_USERNAME): cv.string,
        vol.Required(CONF_PASSWORD): cv.string,
    }
)

SCAN_INTERVAL = timedelta(minutes=60)


def setup_platform(hass, config, add_entities, discovery_info=None):
    """Set up the sensor platform."""

    name = config.get(CONF_NAME)
    username = config.get(CONF_USERNAME)
    password = config.get(CONF_PASSWORD)

    if name is None:
        name = f"package_concierge_{username}"

    scraper = PackageConciergeScraper(username, password)

    add_entities([PackageConciergeSensor(scraper, name)])


class PackageConciergeSensor(Entity):
    """Representation of a Sensor."""

    def __init__(self, scraper, name):
        """Initialize the sensor."""
        self._state = None
        self._scraper = scraper
        self._name = name

    @property
    def name(self):
        """Return the name of the sensor."""
        return self._name

    @property
    def icon(self):
        return "mdi:locker-multiple"

    @property
    def state(self):
        """Return the state of the sensor."""
        return self._state

    @property
    def unit_of_measurement(self):
        """Return the unit of measurement."""
        return "package(s)"

    def update(self):
        """Fetch new state data for the sensor.

        This is the only method that should fetch new data for Home Assistant.
        """
        self._scraper.update()
        self._state = self._scraper.number_of_packages


# The stuff below should be extracted into a Python package, but that's a future problem.
import requests
from bs4 import BeautifulSoup


class PackageConciergeScraper:
    def __init__(self, username, password):
        """Create a new scraper with the given settings."""
        self._username = username
        self._password = password
        self._number_of_packages = None

    @property
    def number_of_packages(self):
        return self._number_of_packages

    def update(self):
        _LOGGER.debug(f"Package Concierge running update for: {self._username}")

        login_url = "https://packageconciergeadmin.com/Login.aspx"

        # Create a persistent session for cookies.
        session = requests.Session()

        login_response = session.get(login_url)
        login_page = BeautifulSoup(login_response.content, features="html.parser")

        # Extract the required fields:
        view_state = login_page.find("input", {"name": "__VIEWSTATE"})["value"]
        view_state_generator = login_page.find(
            "input", {"name": "__VIEWSTATEGENERATOR"}
        )["value"]
        event_validation = login_page.find("input", {"name": "__EVENTVALIDATION"})[
            "value"
        ]

        _LOGGER.debug(
            f"During update for: {self._username}, view_state: {view_state}, view_state_generator: {view_state_generator}, event_validation: {event_validation}."
        )

        form_data = {
            "__VIEWSTATE": view_state,
            "__VIEWSTATEGENERATOR": view_state_generator,
            "__EVENTVALIDATION": event_validation,
            "ctl00$ctl00$MainContent$MainContent$txt_Username": self._username,
            "ctl00$ctl00$MainContent$MainContent$txt_Password": self._password,
            "ctl00$ctl00$MainContent$MainContent$btn_Submit.x": 42,  # These values can be anything, but they must be set.
            "ctl00$ctl00$MainContent$MainContent$btn_Submit.y": 21,
        }
        response = session.post(login_url, data=form_data)

        if response.url != "https://packageconciergeadmin.com/Welcome.aspx":
            _LOGGER.error(
                f"Package Concierge login failed for username: {self._username}"
            )

            raise RuntimeError("Login failed.")

        # Login worked. Use the session to get the member page.
        response = session.get("https://packageconciergeadmin.com/Member/Summary.aspx")
        summary_page = BeautifulSoup(response.content, features="html.parser")

        table = summary_page.find(
            "table",
            {
                "id": "ctl00_ctl00_ctl00_MainContent_MainContent_MainContent_grid_PackageActivityGrid_ctl00"
            },
        )
        rows = table.find_all("tbody")[-1].find_all("tr")
        _LOGGER.debug(f"During update for: {self._username}, all rows: {rows}")

        number_of_packages = 0
        for row in rows:
            if row.find_all("td")[2].text == "Delivered":
                number_of_packages += 1

        self._number_of_packages = number_of_packages
