#include <OneWire.h>
#include <DallasTemperature.h>

// Pin One-Wire où est connecté le DS18B20
#define ONE_WIRE_BUS 2

// On crée un objet OneWire sur la pin définie
OneWire oneWire(ONE_WIRE_BUS);

// On crée un objet DallasTemperature en lui passant l'objet OneWire
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  // Initialisation de la librairie DallasTemperature
  sensors.begin();

  Serial.println("Lecture du capteur DS18B20 via 1-Wire...");
}

void loop() {
  // Envoyer la requête de mesure à tous les capteurs 1-Wire
  sensors.requestTemperatures();

  // Lire la température (en °C) du premier capteur trouvé sur le bus
  float tempC = sensors.getTempCByIndex(0);

  // Vérifier la validité de la mesure
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Erreur : capteur deconnecte ou non detecte!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println(" °C");
  }

  // Attendre quelques secondes avant la prochaine mesure
  delay(2000);
}
