#include <Wire.h>
#include <BH1750.h>

// Création de l'objet capteur
BH1750 lightMeter;

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Attendre l'ouverture du moniteur série

    Serial.println("Test du capteur SEN0562 (BH1750)");

    // Initialisation du capteur
    Wire.begin();
    if (!lightMeter.begin()) {
        Serial.println("Erreur : Capteur SEN0562 non détecté !");
        while (1);
    }

    Serial.println("Capteur SEN0562 initialisé !");
}

void loop() {
    // Lire la luminosité en lux
    float lux = lightMeter.readLightLevel();
    
    Serial.print("Luminosité : ");
    Serial.print(lux);
    Serial.println(" lux");

    delay(1000); // Attente de 1 seconde
}
