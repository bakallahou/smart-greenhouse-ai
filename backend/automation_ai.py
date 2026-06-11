def automation_engine(sensor_data):

    actions = []


    if sensor_data["temperature"] > 30:

        actions.append(
            "Cooling Fan Activated"
        )

    else:

        actions.append(
            "Cooling Fan OFF"
        )


    if sensor_data["soil"] < 40:

        actions.append(
            "Water Pump Activated"
        )

    else:

        actions.append(
            "Water Pump OFF"
        )


    if sensor_data["light"] < 30:

        actions.append(
            "Artificial Light ON"
        )

    else:

        actions.append(
            "Artificial Light OFF"
        )


    if sensor_data["humidity"] < 35:

        actions.append(
            "Humidity Control Activated"
        )

    else:

        actions.append(
            "Humidity Level Stable"
        )


    return actions