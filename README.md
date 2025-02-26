📌 Open Ruche - Suivi Connecté des Abeilles
Un projet IoT pour surveiller la santé des ruches en temps réel 🐝📡
📖 Présentation

Le projet Open Ruche vise à suivre l’évolution des colonies d’abeilles grâce à un système IoT autonome équipé de capteurs.
Les données (température, humidité, poids, luminosité...) sont transmises via LoRaWAN et affichées sur une interface web.
Des alertes (SMS/email) sont envoyées en cas de comportement anormal.
📋 Objectifs

✅ Surveiller la santé des abeilles et leur activité.
✅ Alerter en cas d’essaimage, de baisse de poids, ou de vol de la ruche.
✅ Stocker et visualiser les données sur Ubidots STEM puis BEEP Monitor.
✅ Un système autonome, alimenté par batterie LiPo et panneaux solaires.
🛠️ Matériel Utilisé
1️⃣ Microcontrôleur

    Arduino MKR WAN 1310 (LPWAN LoRaWAN intégré)

2️⃣ Capteurs
Nom	Mesure	Interface	Modèle
HX711	Poids de la ruche (±100g)	Analogique	HX711
DHT22/DHT11	Température et humidité	Digital	DHT22
SEN0562	Luminosité extérieure (lux)	I2C (0x39/0x29)	SEN0562
3️⃣ Communication & Stockage

    LoRaWAN (MKR WAN 1310 → TTN → BEEP Monitor)
    MQTT (pour envoyer les données à Ubidots)

🛠️ Installation et Configuration
1️⃣ Pré-requis

Avant de commencer, installe :

    Arduino IDE
    Bibliothèques Arduino :

    Arduino_LoRa
    Adafruit_Sensor
    BH1750
    DHT

2️⃣ Câblage des capteurs
Capteur	VCC	GND	Signal	Interface
HX711	3.3V	GND	A0	Analogique
DHT22	3.3V	GND	D2	Digital
SEN0562	3.3V	GND	SDA → 11, SCL → 12	I2C
🚀 Déploiement
1️⃣ Téléverser le code sur l’Arduino

    Branche ton MKR WAN 1310 en USB.
    Ouvre Arduino IDE, sélectionne MKR WAN 1310.
    Téléverse le programme principal.

2️⃣ Tester les capteurs

    Ouvre le Moniteur Série (115200 bauds)
    Vérifie que chaque capteur renvoie des valeurs correctes :
        Luminosité : Luminosité : 120 lux
        Température : Température : 25.3°C
        Poids : Poids : 12.4 kg

3️⃣ Vérifier l’envoi des données

    LoRaWAN : Vérifie la réception des données sur TTN (The Things Network).
    Ubidots STEM : Vérifie les graphiques en temps réel.
    BEEP Monitor : Vérifie la centralisation des données.

📡 Affichage des Données
🌍 Interface Web

    Développement : Visualisation via Ubidots STEM.
    Rendu final : Données intégrées à BEEP Monitor.

📢 Alertes & Notifications

⚠️ Le système enverra des alertes si :

    🐝 Essaimage détecté (variation anormale du poids).
    🔥 Température hors plage (-10°C à 85°C).
    📉 Baisse du poids rapide (vol de la ruche).
    🔋 Batterie faible (sous 20%).

🔔 Envoi des notifications via SMS/Email (Ubidots Webhook).
📦 Roadmap & Améliorations

✅ Prototype sur breadboard terminé.
✅ Test Unitaire de chaque capteur opérationnelle.
✅ Connexion TTN opérationnelle.
🔲 Conception du PCB et boîtier étanche pour extérieur.
🔲 Optimisation de la consommation énergétique (deep sleep).
👥 Contributeurs

    Léa LACOUTURE - Cheffe de projet 👩‍💻
    Amine OUMERT - Specialist, Implémenter
    Gloire A Dieu DEMBI - Plant, Teamworker

    [Équipe EI4 FISA : Polytech Sorbonne - Système Embarqué IoT]

🔗 Contact : 📧 lealacouture@live.fr
             📧 oumertamine3@gmail.com
             📧 gloiredembi@gmail.com

📌 Résumé rapide

🚀 Un système IoT complet pour surveiller la santé des ruches en temps réel.
📊 Affichage des données sur Ubidots et BEEP Monitor.
📡 Communication via LoRaWAN & MQTT.
⚠️ Alertes automatiques en cas de comportement anormal.

🐝 Sauvegardons les abeilles avec la technologie ! 🌍💡
