import random


def generate_sensor_data():

    data = {

        "temperature": random.randint(20, 40),

        "humidity": random.randint(40, 90),

        "soil": random.randint(20, 80),

        "light": random.randint(100, 1000)

    }

    return data