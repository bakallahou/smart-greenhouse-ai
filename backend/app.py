from fastapi import (
    FastAPI,
    Request,
    UploadFile,
    File,
    WebSocket
)

from fastapi.responses import HTMLResponse

from fastapi.templating import Jinja2Templates

from fastapi.staticfiles import StaticFiles

from backend.simulator import generate_sensor_data

from backend.ai_engine import ai_decision

from backend.disease_ai import detect_disease

from backend.recommendation_ai import (
    generate_recommendation
)

from backend.alert_ai import generate_alerts

from backend.automation_ai import (
    automation_engine
)

from backend.report_generator import (
    generate_pdf_report
)

from backend.database import (
    sensor_collection,
    disease_collection
)

from backend.esp_api import (
    router as esp_router,
    latest_sensor_data
)

import shutil
import asyncio

app = FastAPI()

app.include_router(esp_router)

app.mount(
    "/uploads",
    StaticFiles(directory="uploads"),
    name="uploads"
)

app.mount(
    "/static",
    StaticFiles(directory="static"),
    name="static"
)

templates = Jinja2Templates(
    directory="templates"
)


@app.get("/", response_class=HTMLResponse)

async def dashboard(request: Request):

    if latest_sensor_data["temperature"] == 0:
        sensor_data = generate_sensor_data()
    else:
        sensor_data = latest_sensor_data

    decisions = ai_decision(sensor_data)

    automation_actions = automation_engine(
        sensor_data
    )

    latest_sensors = list(

        sensor_collection.find().sort(
            "_id",
            -1
        ).limit(5)

    )

    latest_diseases = list(

        disease_collection.find().sort(
            "_id",
            -1
        ).limit(5)

    )

    return templates.TemplateResponse(

        request=request,

        name="index.html",

        context={

            "temperature": sensor_data["temperature"],

            "humidity": sensor_data["humidity"],

            "soil": sensor_data["soil"],

            "light": sensor_data["light"],

            "pump": decisions["pump"],

            "fan": decisions["fan"],

            "lamp": decisions["lamp"],

            "heater": decisions["heater"],

            "automation_actions": automation_actions,

            "disease_result": "",

            "recommendations": [],

            "alerts": [],

            "image_path": "",

            "latest_sensors": latest_sensors,

            "latest_diseases": latest_diseases

        }

    )


@app.websocket("/ws")

async def websocket_endpoint(websocket: WebSocket):

    await websocket.accept()

    try:
        while True:

            if latest_sensor_data["temperature"] == 0:

                sensor_data = generate_sensor_data()

            else:

                sensor_data = latest_sensor_data

            decisions = ai_decision(sensor_data)

            sensor_collection.insert_one({

                "temperature": sensor_data["temperature"],

                "humidity": sensor_data["humidity"],

                "soil": sensor_data["soil"],

                "light": sensor_data["light"],

                "pump": decisions["pump"],

                "fan": decisions["fan"],

                "lamp": decisions["lamp"],

                "heater": decisions["heater"]

            })

            await websocket.send_json({

                "temperature": sensor_data["temperature"],

                "humidity": sensor_data["humidity"],

                "soil": sensor_data["soil"],

                "light": sensor_data["light"],

                "pump": decisions["pump"],

                "fan": decisions["fan"],

                "lamp": decisions["lamp"],

                "heater": decisions["heater"]

            })

            await asyncio.sleep(2)
    except:
        pass


@app.post("/upload")

async def upload_image(

    request: Request,

    file: UploadFile = File(...)

):

    file_path = f"uploads/{file.filename}"

    with open(file_path, "wb") as buffer:

        shutil.copyfileobj(file.file, buffer)

    disease_result = detect_disease(file_path)

    recommendations = generate_recommendation(
        disease_result
    )

    # 2️⃣ CORRECTION ICI : Utilisation de la logique de bascule ESP32 / Simulateur
    if latest_sensor_data["temperature"] == 0:
        sensor_data = generate_sensor_data()
    else:
        sensor_data = latest_sensor_data

    alerts = generate_alerts(
        sensor_data,
        disease_result
    )

    generate_pdf_report(

        disease_result,

        recommendations,

        alerts,

        sensor_data

    )

    disease_collection.insert_one({

        "image": file.filename,

        "result": disease_result

    })

    decisions = ai_decision(sensor_data)

    automation_actions = automation_engine(
        sensor_data
    )

    latest_sensors = list(

        sensor_collection.find().sort(
            "_id",
            -1
        ).limit(5)

    )

    latest_diseases = list(

        disease_collection.find().sort(
            "_id",
            -1
        ).limit(5)

    )

    return templates.TemplateResponse(

        request=request,

        name="index.html",

        context={

            "temperature": sensor_data["temperature"],

            "humidity": sensor_data["humidity"],

            "soil": sensor_data["soil"],

            "light": sensor_data["light"],

            "pump": decisions["pump"],

            "fan": decisions["fan"],

            "lamp": decisions["lamp"],

            "heater": decisions["heater"],

            "automation_actions": automation_actions,

            "disease_result": disease_result,

            "recommendations": recommendations,

            "alerts": alerts,

            "image_path": file_path,

            "latest_sensors": latest_sensors,

            "latest_diseases": latest_diseases

        }

    )