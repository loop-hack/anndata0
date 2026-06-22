import requests
from requests.auth import HTTPBasicAuth

DITTO_BASE_URL = "http://localhost:8080/api/2/things"

AUTH = HTTPBasicAuth("ditto", "ditto")


def thing_id_for_farm(farm_id: str) -> str:
    """
    Map a farm_id (e.g. "farm01") to its full Ditto thing id
    (e.g. "smartfarm:farm01"). There is no fallback "legacy" farm
    anymore — a missing/blank farm_id is a caller error, not something
    to silently paper over with a default thing, since that default was
    exactly what made a stale farm appear even after every real farm was
    deleted.
    """
    if not farm_id:
        raise ValueError("farm_id is required — there is no default/legacy farm")
    return f"smartfarm:{farm_id}"


def thing_exists(thing_id: str) -> bool:
    response = requests.get(f"{DITTO_BASE_URL}/{thing_id}", auth=AUTH)
    return response.status_code == 200


def get_twin(thing_id: str):
    response = requests.get(f"{DITTO_BASE_URL}/{thing_id}", auth=AUTH)
    response.raise_for_status()
    return response.json()


def get_actual(thing_id: str):
    return get_twin(thing_id).get("features", {}).get("actual", {}).get("properties", {})


def get_virtual(thing_id: str):
    return get_twin(thing_id).get("features", {}).get("virtual", {}).get("properties", {})


def get_attributes(thing_id: str):
    return get_twin(thing_id).get("attributes", {})


def get_scenarios(thing_id: str):
    """Returns the farm's saved scenarios — {} if none have ever been saved."""
    return get_twin(thing_id).get("features", {}).get("scenarios", {}).get("properties", {})