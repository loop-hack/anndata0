import requests
from requests.auth import HTTPBasicAuth

DITTO_BASE_URL = "http://localhost:8080/api/2/things"
THING_ID = "smartfarm:twin01"

AUTH = HTTPBasicAuth(
    "ditto",
    "ditto"
)


def update_virtual_property(name, value):

    response = requests.put(
        f"{DITTO_BASE_URL}/{THING_ID}/features/virtual/properties/{name}",
        auth=AUTH,
        json=value
    )

    response.raise_for_status()

    return {
        "status": "success",
        "property": name,
        "value": value
    }


def update_actual_property(name, value):

    response = requests.put(
        f"{DITTO_BASE_URL}/{THING_ID}/features/actual/properties/{name}",
        auth=AUTH,
        json=value
    )

    response.raise_for_status()

    return {
        "status": "success",
        "property": name,
        "value": value
    }