#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

// Création de l'objet capteur
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(0x29, 12345);

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Attendre l'ouverture du moniteur série

    Serial.println("Test du capteur TSL2561");

    if (!tsl.begin()) {
        Serial.println("Erreur : Capteur TSL2561 non détecté !");
        while (1);
    }

    // Configurer le gain et le temps d'intégration
    tsl.setGain(TSL2561_GAIN_16X); // Gain x16 pour faible luminosité
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Temps d'intégration rapide

    Serial.println("Capteur initialisé !");
}

void loop() {
    sensors_event_t event;
    tsl.getEvent(&event);

    if (event.light) {
        Serial.print("Luminosité : ");
        Serial.print(event.light);
        Serial.println(" lux");
    } else {
        Serial.println("Erreur : Impossible de lire la valeur !");
    }

    delay(1000);  // Attendre 1 seconde avant la prochaine mesure
}
