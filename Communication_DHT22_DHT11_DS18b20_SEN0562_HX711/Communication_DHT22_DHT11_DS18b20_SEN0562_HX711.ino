#include <MKRWAN.h>          // Communication LoRa
#include <Wire.h>            // Communication I2C
#include "ArduinoLowPower.h" // Gestion de l'alimentation
#include <DHT.h>             // Capteurs DHT
#include <OneWire.h>         // Communication 1-Wire (DS18B20)
#include <DallasTemperature.h> // Capteur DS18B20
#include <BH1750.h>          // Capteur de luminosité BH1750
#include "HX711.h"           // Capteur de poids HX711

////////////////////////
///     SETUP      ///
////////////////////////

///// INITIALISATION LORAWAN - CONNEXION À TTN /////
LoRaModem modem;
#define SECRET_APP_EUI "0000000000000001"
#define SECRET_APP_KEY "8062D07A1C984423278A7D938C73E534"
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

///// INITIALISATION DES CAPTEURS /////
// Définition des broches
#define DHT22_PIN 2   // Capteur DHT22
#define DHT11_PIN 3   // Capteur DHT11
#define DS18B20_PIN 7 // Capteur DS18B20
#define LOADCELL_DOUT_PIN 0  // Data HX711
#define LOADCELL_SCK_PIN 1   // Clock HX711

// Initialisation des capteurs
DHT dht22(DHT22_PIN, DHT22);
DHT dht11(DHT11_PIN, DHT11);
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);
BH1750 lightMeter; // Capteur BH1750
HX711 scale; // Capteur de poids HX711

///// VARIABLES GLOBALES /////
float tempDHT22, humDHT22;
float tempDHT11, humDHT11;
float tempDS18B20;
float luminosite;
float poids;
float calibration_factor = 30226.0;  // Facteur de calibration HX711
byte buffer[14]; // Stocke les valeurs à envoyer (2 octets * 7 valeurs)

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
  Serial.println("=== Démarrage du système ===");

  connection();
  
  dht22.begin();
  dht11.begin();
  ds18b20.begin(); // Initialisation du DS18B20

  Wire.begin();
  if (!lightMeter.begin()) {
    Serial.println("Erreur : Le capteur BH1750 n'est pas détecté !");
    while (1);
  }
  Serial.println("Capteur BH1750 détecté avec succès.");

  // Initialisation du capteur HX711 (poids)
  Serial.println("Initialisation du capteur HX711...");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  delay(5000);
  scale.tare();  
  scale.set_scale(calibration_factor);
  Serial.println("Capteur HX711 prêt !");

  delay(5000);

  pinMode(LED_BUILTIN, OUTPUT);
}

////////////////////////
///      LOOP      ///
////////////////////////

void measureDHTSensors() {
  Serial.println("=== Lecture des capteurs DHT ===");
  humDHT22 = dht22.readHumidity();
  tempDHT22 = dht22.readTemperature();
  humDHT11 = dht11.readHumidity();
  tempDHT11 = dht11.readTemperature();

  if (isnan(humDHT22) || isnan(tempDHT22) || isnan(humDHT11) || isnan(tempDHT11)) {
    Serial.println("Erreur de lecture DHT !");
    return;
  }

  Serial.print("Température DHT22 : "); Serial.print(tempDHT22); Serial.println(" °C");
  Serial.print("Humidité DHT22 : "); Serial.print(humDHT22); Serial.println(" %");
  Serial.print("Température DHT11 : "); Serial.print(tempDHT11); Serial.println(" °C");
  Serial.print("Humidité DHT11 : "); Serial.print(humDHT11); Serial.println(" %");

  buildMessage(tempDHT22, 0);
  buildMessage(humDHT22, 2);
  buildMessage(tempDHT11, 4);
  buildMessage(humDHT11, 6);
}

void measureDS18B20() {
  Serial.println("=== Lecture du capteur DS18B20 ===");
  ds18b20.requestTemperatures();
  tempDS18B20 = ds18b20.getTempCByIndex(0);

  if (tempDS18B20 == DEVICE_DISCONNECTED_C) {
    Serial.println("Erreur de lecture DS18B20 !");
    return;
  }

  Serial.print("Température DS18B20 : "); Serial.print(tempDS18B20); Serial.println(" °C");
  buildMessage(tempDS18B20, 8);
}

void measureLuminosity() {
  Serial.println("=== Lecture du capteur de luminosité BH1750 ===");
  luminosite = lightMeter.readLightLevel();

  Serial.print("Luminosité : "); Serial.print(luminosite); Serial.println(" Lux");
  buildMessage(luminosite, 10);
}

void measureWeight() {
  Serial.println("=== Lecture du capteur de poids HX711 ===");
  
  if (scale.is_ready()) {
    poids = scale.get_units(10);
    Serial.print("Poids mesuré : "); Serial.print(poids, 2); Serial.println(" kg");
    buildMessage(poids, 12);
  } else {
    Serial.println("Erreur : Capteur HX711 non détecté !");
  }
}

// Stocke chaque mesure sur 2 octets (valeur * 100 pour garder 2 décimales)
void buildMessage(float donnee_float, int index) {
  short donnee_short = (short)(donnee_float * 100);
  buffer[index] = donnee_short & 0xFF;
  buffer[index + 1] = (donnee_short >> 8) & 0xFF;
}

void sendMessage() {
  Serial.println("=== ENVOI DES DONNÉES LoRa ===");

  modem.beginPacket();
  modem.write(buffer, 14); 
  int err = modem.endPacket(true);

  if (err > 0) {
    Serial.println("Message LoRa envoyé avec succès !");
  } else {
    Serial.println("Échec de l'envoi !");
  }
}

void loop() {
  Serial.println("\n=== Début d'un nouveau cycle ===");
  
  measureDHTSensors();
  measureDS18B20();
  measureLuminosity();
  measureWeight();

  sendMessage();
  
  Serial.println("Mise en veille pour économiser l'énergie...");
  LowPower.deepSleep(10); // 10 minutes
}
