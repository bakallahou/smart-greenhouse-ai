Projet : Smart Greenhouse AI
Un système IoT et d'Intelligence Artificielle de niveau ingénieur pour l'automatisation et le diagnostic autonome d'une serre agricole.

Couche Embarquée (ESP32-S3) :

Acquisition : Lecture en temps réel de la température/humidité (DHT22), de l'humidité du sol et de la luminosité (LDR).

Régulation Locale : Contrôle automatique non-bloquant (millis()) de 4 relais (pompe, ventilateur, lampe horticole).

IHM : Affichage dynamique alterné sur écran LCD I2C 16x2.

Couche Serveur (FastAPI & MongoDB) :

Flux IoT : Réception des données capteurs par requêtes HTTP POST JSON et stockage sur MongoDB.

Supervision : Dashboard web synchronisé en temps réel avec le matériel via une connexion WebSocket.

Couche Intelligence Artificielle (Deep Learning) :

Vision par Ordinateur : Prise de vue automatique toutes les 5 minutes via la caméra OV2640 et transfert binaire multipart/form-data.

Diagnostic : Analyse instantanée de l'image par un modèle TensorFlow (plant_disease_model.h5) pour détecter les maladies de la tomate (Healthy, Early Blight, Late Blight).

Décisionnel : Génération automatique d'alertes, de conseils agricoles et de rapports d'état complets en format PDF.
