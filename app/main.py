from fastapi import FastAPI, Body
from pydantic import BaseModel
from fastapi.responses import FileResponse

from app.ditto_reader import (
    get_twin,
    get_actual,
    get_virtual,
    get_attributes
)

from app.ditto_writer import (
    update_virtual_property,
    update_actual_property
)

app = FastAPI(
    title="Smart Farm Digital Twin",
    version="2.0.0"
)


#models

class PropertyUpdate(BaseModel):
    property: str
    value: object


#basic endpoints

@app.get("/")
def root():
    return {
        "project": "Smart Farm Digital Twin",
        "architecture": "Ditto First",
        "status": "running"
    }


@app.get("/health")
def health():
    return {
        "backend": "healthy",
        "source": "Eclipse Ditto"
    }


#reading endpoints

@app.get("/farm/twin")
def farm_twin():
    return get_twin()


@app.get("/farm/digital-twin")
def digital_twin():
    return {
        "attributes": get_attributes(),
        "actual": get_actual(),
        "virtual": get_virtual()
    }


@app.get("/farm/attributes")
def attributes():
    return get_attributes()


@app.get("/farm/actual")
def actual():
    return get_actual()


@app.get("/farm/virtual")
def virtual():
    return get_virtual()


#writer endpoints

@app.post("/farm/virtual")
def update_virtual(payload: PropertyUpdate):

    return update_virtual_property(
        payload.property,
        payload.value
    )


@app.post("/farm/actual")
def update_actual(payload: PropertyUpdate):

    return update_actual_property(
        payload.property,
        payload.value
    )



@app.get("/dashboard")
def dashboard():
    return FileResponse(
        "templates/dashboard.html"
    )