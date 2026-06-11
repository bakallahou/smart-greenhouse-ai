def ai_decision(data):

    decisions = {

        "pump": "OFF",
        "fan": "OFF",
        "lamp": "OFF",
        "heater": "OFF"

    }

    # Temperature control

    if data["temperature"] > 30:
        decisions["fan"] = "ON"

    if data["temperature"] < 22:
        decisions["heater"] = "ON"

    # Soil moisture control

    if data["soil"] < 30:
        decisions["pump"] = "ON"

    # Light control

    if data["light"] < 300:
        decisions["lamp"] = "ON"

    return decisions