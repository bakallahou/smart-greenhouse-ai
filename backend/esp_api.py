from fastapi import APIRouter
from backend.database import sensor_collection

router = APIRouter()

latest_sensor_data = {

    "temperature": 0,
    "humidity": 0,
    "soil": 0,
    "light": 0

}


@router.post("/esp32/data")

async def receive_sensor_data(data: dict):

    latest_sensor_data["temperature"] = data["temperature"]

    latest_sensor_data["humidity"] = data["humidity"]

    latest_sensor_data["soil"] = data["soil"]

    latest_sensor_data["light"] = data["light"]

    sensor_collection.insert_one({

        "temperature": data["temperature"],
        "humidity": data["humidity"],
        "soil": data["soil"],
        "light": data["light"]

    })

    return {

        "message": "ESP32 Sensor Data Received Successfully"

    }