import requests
from requests.auth import HTTPBasicAuth

DITTO_BASE_URL = "http://localhost:8080/api/2/things"
THING_ID = "smartfarm:twin01"

AUTH = HTTPBasicAuth(
    "ditto",
    "ditto"
)


def get_twin():
    """
    Return complete twin01 object.
    """
    response = requests.get(
        f"{DITTO_BASE_URL}/{THING_ID}",
        auth=AUTH
    )

    response.raise_for_status()

    return response.json()


def get_actual():
    """
    Return actual sensor values.
    """
    twin = get_twin()
    return twin["features"]["actual"]["properties"]


def get_virtual():
    """
    Return virtual sensor values.
    """
    twin = get_twin()
    return twin["features"]["virtual"]["properties"]


def get_attributes():
    """
    Return farm metadata.
    """
    twin = get_twin()
    return twin["attributes"]