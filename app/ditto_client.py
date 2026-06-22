import requests
from requests.auth import HTTPBasicAuth

DITTO = "http://localhost:8080/api/2/things"
AUTH = HTTPBasicAuth("ditto", "ditto")

HEADERS_MERGE = {"Content-Type": "application/merge-patch+json"}


def put_thing(payload: dict):
    url = f"{DITTO}/{payload['thingId']}"
    r = requests.put(url, json=payload, auth=AUTH)
    if not r.ok:
        raise Exception(r.text)
    return r.json()


def delete_thing(thing_id: str):
    r = requests.delete(f"{DITTO}/{thing_id}", auth=AUTH)
    if not r.ok:
        raise Exception(r.text)
    return {"deleted": thing_id}


def get_json(url):
    r = requests.get(url, auth=AUTH)
    if r.status_code == 404:
        return None
    r.raise_for_status()
    return r.json()


def patch_feature(thing_id: str, feature: str, props: dict):
    url = f"{DITTO}/{thing_id}/features/{feature}"

    # auto-create feature if missing
    if requests.get(url, auth=AUTH).status_code == 404:
        requests.put(url, json={"properties": {}}, auth=AUTH)

    r = requests.patch(url, json={"properties": props}, auth=AUTH, headers=HEADERS_MERGE)
    if not r.ok:
        raise Exception(r.text)

    return r.json()