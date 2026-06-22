import re
import requests
from requests.auth import HTTPBasicAuth

DITTO_SEARCH_URL = "http://localhost:8080/api/2/search/things"
AUTH = HTTPBasicAuth("ditto", "ditto")

EXCLUDED = {"smartfarm:sensor_catalog"}
FARM_PATTERN = re.compile(r"^smartfarm:farm(\d+)$")
SENSOR_PATTERN = re.compile(r"^smartfarm:s\d+$")


def list_farm_thing_ids():
    """Pure pattern search: every smartfarm:farmNN thing. Does NOT include
    the legacy thing (smartfarm:twin01) — that's a different naming
    convention by design. Callers that need the full farm list (legacy +
    farmNN) should use farm_registry.list_farm_thing_ids(), not this."""
    res = requests.get(
        DITTO_SEARCH_URL,
        auth=AUTH,
        params={
            "filter": 'like(thingId,"smartfarm:farm*")',
            "fields": "thingId",
            "option": "size(200)"
        }
    )
    res.raise_for_status()

    return [
        i["thingId"]
        for i in res.json().get("items", [])
        if i["thingId"] not in EXCLUDED
        and FARM_PATTERN.match(i["thingId"])
    ]


def list_sensor_thing_ids():
    """Pure pattern search: every smartfarm:sNN thing. Not yet consumed
    anywhere — available for future sensor cleanup/admin tooling."""
    res = requests.get(
        DITTO_SEARCH_URL,
        auth=AUTH,
        params={
            "filter": 'like(thingId,"smartfarm:s*")',
            "fields": "thingId",
            "option": "size(200)"
        }
    )
    res.raise_for_status()

    return [
        i["thingId"]
        for i in res.json().get("items", [])
        if SENSOR_PATTERN.match(i["thingId"])
    ]