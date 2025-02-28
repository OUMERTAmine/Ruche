#include <MKRWAN.h>          // Communication LoRa
#include <Wire.h>
#include "ArduinoLowPower.h" // Gestion de l'alimentation
#include <DHT.h>             // Capteurs Température & Humidité

////////////////////////
////     SETUP      ////
////////////////////////

///// INITIALISATION LORAWAN - CONNEXION À TTN /////
LoRaModem modem;
#define SECRET_APP_EUI "0000000000000001"
#define SECRET_APP_KEY "8062D07A1C984423278A7D938C73E534"
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

///// INITIALISATION DES CAPTEURS /////
// Définition des broches des capteurs
#define DHT22_PIN 2   // Capteur DHT22 sur broche 2
#define DHT11_PIN 3  // Capteur DHT11 sur broche A1

DHT dht22(DHT22_PIN, DHT22);
DHT dht11(DHT11_PIN, DHT11);

///// VARIABLES GLOBALES /////
float tempDHT22, humDHT22;
float tempDHT11, humDHT11;
byte buffer[8]; // Stocke les valeurs à envoyer (2 octets * 4 valeurs)
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
  
  dht22.begin();
  dht11.begin();

  pinMode(LED_BUILTIN, OUTPUT);
}

////////////////////////
////      LOOP      ////
////////////////////////

///// MESURE TEMPÉRATURE & HUMIDITÉ - DHT22 & DHT11 /////
void measureDHTSensors() {
  Serial.println("Lecture du DHT22...");
  humDHT22 = dht22.readHumidity();
  tempDHT22 = dht22.readTemperature();

  Serial.println("Lecture du DHT11...");
  humDHT11 = dht11.readHumidity();
  tempDHT11 = dht11.readTemperature();

  if (isnan(humDHT22) || isnan(tempDHT22) || isnan(humDHT11) || isnan(tempDHT11)) {
    Serial.println("Erreur de lecture DHT !");
    return;
  }

  Serial.print("Température DHT22 : ");
  Serial.print(tempDHT22);
  Serial.println(" °C");
  
  Serial.print("Humidité DHT22 : ");
  Serial.print(humDHT22);
  Serial.println(" %");

  Serial.print("Température DHT11 : ");
  Serial.print(tempDHT11);
  Serial.println(" °C");
  
  Serial.print("Humidité DHT11 : ");
  Serial.print(humDHT11);
  Serial.println(" %");

  // Stockage dans le buffer
  buildMessage(tempDHT22, 0);
  buildMessage(humDHT22, 2);
  buildMessage(tempDHT11, 4);
  buildMessage(humDHT11, 6);
}

///// ENCODAGE DES DONNÉES EN FORMAT BINAIRE /////
// Stocke chaque mesure sur 2 octets (valeur * 100 pour garder 2 décimales)
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
  modem.write(buffer, 8); // 8 octets (2 octets * 4 valeurs)
  err = modem.endPacket(true);

  if (err > 0) {
    Serial.println("Message envoyé avec succès !");
  } else {
    Serial.println("Échec de l'envoi !");
  }
}

///// BOUCLE PRINCIPALE /////
void loop() {
  measureDHTSensors();
  sendMessage();
  
  Serial.println("Mise en veille pour économiser l'énergie...");
  LowPower.deepSleep(100); // 10 minutes
}
