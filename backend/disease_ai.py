import tensorflow as tf

import numpy as np

from tensorflow.keras.preprocessing import image


model = tf.keras.models.load_model(
    "ai_models/plant_disease_model.h5"
)


classes = [

    "Tomato_Early_blight",

    "Tomato_Late_blight",

    "Tomato_healthy"

]


def detect_disease(image_path):

    img = image.load_img(

        image_path,

        target_size=(128,128)

    )

    img_array = image.img_to_array(img)

    img_array = np.expand_dims(img_array, axis=0)

    img_array = img_array / 255.0

    prediction = model.predict(img_array)

    predicted_class = classes[np.argmax(prediction)]

    confidence = np.max(prediction) * 100

    return f"{predicted_class} ({confidence:.2f}%)"