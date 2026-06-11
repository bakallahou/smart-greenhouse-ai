import tensorflow as tf

from tensorflow.keras.models import Sequential

from tensorflow.keras.layers import (

    Conv2D,
    MaxPooling2D,
    Flatten,
    Dense

)

from tensorflow.keras.preprocessing.image import ImageDataGenerator


train_data = ImageDataGenerator(

    rescale=1./255,

    validation_split=0.2

)


train_generator = train_data.flow_from_directory(

    "datasets/plant_disease",

    target_size=(128,128),

    batch_size=32,

    class_mode="categorical",

    subset="training"

)


val_generator = train_data.flow_from_directory(

    "datasets/plant_disease",

    target_size=(128,128),

    batch_size=32,

    class_mode="categorical",

    subset="validation"

)


model = Sequential([

    Conv2D(
        32,
        (3,3),
        activation="relu",
        input_shape=(128,128,3)
    ),

    MaxPooling2D(2,2),

    Conv2D(
        64,
        (3,3),
        activation="relu"
    ),

    MaxPooling2D(2,2),

    Flatten(),

    Dense(
        128,
        activation="relu"
    ),

    Dense(
        3,
        activation="softmax"
    )

])


model.compile(

    optimizer="adam",

    loss="categorical_crossentropy",

    metrics=["accuracy"]

)


model.fit(

    train_generator,

    validation_data=val_generator,

    epochs=5

)


model.save("ai_models/plant_disease_model.h5")


print("MODEL TRAINED SUCCESSFULLY")