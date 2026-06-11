from pymongo import MongoClient

client = MongoClient("mongodb://localhost:27017")

db = client["smart_greenhouse_ai"]

sensor_collection = db["sensor_data"]

disease_collection = db["disease_detection"]