def generate_recommendation(disease):

    if "Early_blight" in disease:

        return [

            "Reduce watering",

            "Improve air circulation",

            "Use fungicide treatment",

            "Remove infected leaves"

        ]


    elif "Late_blight" in disease:

        return [

            "Stop excessive humidity",

            "Activate greenhouse fan",

            "Apply copper fungicide",

            "Monitor nearby plants"

        ]


    elif "healthy" in disease:

        return [

            "Plant is healthy",

            "Maintain current conditions",

            "Continue monitoring"

        ]


    else:

        return [

            "No recommendation available"

        ]