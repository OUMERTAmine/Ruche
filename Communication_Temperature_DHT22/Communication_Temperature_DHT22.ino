#include <MKRWAN.h>          // Communication LoRa
#include <Wire.h>
#include "ArduinoLowPower.h" // Gestion de l'alimentation
#include <DHT.h>             // Température + Humidité

////////////////////////
////     SETUP      ////
////////////////////////

///// INITIALISATION LORAWAN - CONNEXION À TTN /////
LoRaModem modem;
#define SECRET_APP_EUI "0000000000000001"
#define SECRET_APP_KEY "8062D07A1C984423278A7D938C73E534"
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

///// INITIALISATION DU CAPTEUR DHT22 /////
// Définition de la broche du capteur
#define DHT_PIN 2
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

///// VARIABLES GLOBALES /////
float temperature, humidite;
byte buffer[4]; // Stocke les valeurs à envoyer
int iteration = 0; // Position d'écriture dans buffer

///// FONCTION DE CONNEXION À TTN /////
void connection() {
  if (!modem.begin(EU868)) {
    Serial.println("Échec de l'initialisation LoRa !");
    while (1);
  }
  Serial.print("Version du module : ");
  Serial.println(modem.version());
  Serial.print("Device EUI : ");
  Serial.println(modem.deviceEUI());
  
  if (!modem.joinOTAA(appEui, appKey)) {
    Serial.println("Échec de la connexion à TTN !");
    while (1);
  }
  Serial.println("Connecté à TTN !");
}

///// INITIALISATION AU DÉMARRAGE /////
void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Démarrage...");
  
  connection();
  dht.begin();
  pinMode(LED_BUILTIN, OUTPUT);
}

////////////////////////
////      LOOP      ////
////////////////////////

///// MESURE TEMPÉRATURE & HUMIDITÉ - DHT22 /////
void measureDHT22() {
  Serial.println("Lecture du DHT22...");
  humidite = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidite) || isnan(temperature)) {
    Serial.println("Erreur de lecture DHT22 !");
    return;
  }

  Serial.print("Température : ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("Humidité : ");
  Serial.print(humidite);
  Serial.println(" %");

  buildMessage(temperature, 0); // Stockage température
  buildMessage(humidite, 2);    // Stockage humidité
}

///// ENCODAGE DES DONNÉES EN FORMAT BINAIRE /////
void buildMessage(float donnee_float, int index) {
  short donnee_short = (short)(donnee_float * 100); // Conversion en entier avec 2 décimales
  buffer[index] = donnee_short & 0xFF;        // Byte de poids faible
  buffer[index + 1] = (donnee_short >> 8) & 0xFF; // Byte de poids fort
}

///// ENVOI DES DONNÉES À TTN /////
void sendMessage() {
  Serial.println("===== ENVOI DES DONNÉES =====");
  int err;

  modem.beginPacket();
  modem.write(buffer, 4);
  err = modem.endPacket(true);

  if (err > 0) {
    Serial.println("Message envoyé avec succès !");
  } else {
    Serial.println("Échec de l'envoi !");
  }
}

///// BOUCLE PRINCIPALE /////
void loop() {
  measureDHT22();
  sendMessage();
  
 Serial.println("Mise en veille pour économiser l'énergie...");
 LowPower.deepSleep(100); // 10 minutes
}
