
#include "DHT.h"

// Broche de données du DHT11
#define DHTPIN 2        // Broche numérique D2

// Type du capteur
#define DHTTYPE DHT11

// Création d'une instance de DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialisation de la liaison série
  Serial.begin(9600);
  while (!Serial) {
    ; // Attendre l'ouverture du moniteur série
  }

  // Initialisation du capteur
  Serial.println("Initialisation du DHT11...");
  dht.begin();
}

void loop() {
  // Lecture de l'humidité
  float h = dht.readHumidity();
  // Lecture de la température en °C
  float t = dht.readTemperature();

  // Vérification des valeurs
  if (isnan(h) || isnan(t)) {
    Serial.println("Erreur de lecture du DHT11!");
  } else {
    Serial.print("Humidite: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" °C");
  }

  // DHT11 ne supporte pas des lectures trop rapprochées
  // Attendre 2 secondes avant la prochaine mesure
  delay(2000);
}
