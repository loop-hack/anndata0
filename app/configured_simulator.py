"""
configured_simulator.py

For every catalog sensor that is source == "virtual" *because the user
turned off a real sensor* (as opposed to a sensor that was always
virtual, e.g. weather/plant fields seeded from a farm's own virtual
properties), generates a random value inside the user-given offRange
and writes it into the mapped farm's SEPARATE 'configured' container —
never touching 'actual' (real-only) or 'virtual' (weather/plant sim).

A sensor only gets simulated here if it has BOTH:
  - source == "virtual"
  - wasRealOverride == true   (set when the user flips a real sensor off)
  - offRange set (min/max from the user)
  - farmId set (mapped to a farm)
"""

import random
import time

import requests

from app.sensor_registry import get_all_sensors
from app.ditto_writer import DITTO_BASE_URL, AUTH, MERGE_HEADERS
from app.ditto_reader import thing_id_for_farm


def _random_value(min_v: float, max_v: float, decimals: int = 1) -> float:
    return round(random.uniform(min_v, max_v), decimals)


def tick_once():
    try:
        sensors = get_all_sensors()
    except Exception as e:
        print("configured_simulator: could not read catalog:", e)
        return

    by_farm = {}

    for s in sensors:
        if not s.get("wasRealOverride"):
            continue
        off_range = s.get("offRange")
        farm_id = s.get("farmId")
        if not off_range or not farm_id:
            continue

        prop_name = s.get("sensorType") or s["key"]
        lo = off_range.get("min", 0)
        hi = off_range.get("max", lo + 1)
        decimals = off_range.get("decimals", 1)

        by_farm.setdefault(farm_id, {})[prop_name] = _random_value(lo, hi, decimals)

    for farm_id, properties in by_farm.items():
        thing_id = thing_id_for_farm(farm_id)
        try:
            response = requests.patch(
                f"{DITTO_BASE_URL}/{thing_id}/features/configured/properties",
                auth=AUTH, json=properties, headers=MERGE_HEADERS
            )
            response.raise_for_status()
        except Exception as e:
            print(f"configured_simulator: could not write farm {thing_id}:", e)


def start_configured_simulator(interval_seconds: int = 5):
    while True:
        try:
            tick_once()
        except Exception as e:
            print("configured_simulator loop error:", e)
        time.sleep(interval_seconds)