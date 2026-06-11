def generate_alerts(sensor_data, disease):

    alerts = []


    if sensor_data["temperature"] > 35:

        alerts.append(
            "High Temperature Detected"
        )


    if sensor_data["humidity"] < 30:

        alerts.append(
            "Low Humidity Detected"
        )


    if sensor_data["soil"] < 30:

        alerts.append(
            "Soil Moisture Too Low"
        )


    if sensor_data["light"] < 20:

        alerts.append(
            "Low Light Level"
        )


    if "blight" in disease.lower():

        alerts.append(
            "Plant Disease Detected"
        )


    return alerts