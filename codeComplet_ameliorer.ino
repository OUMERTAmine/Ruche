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
#define DHT11_PIN 3   // Capteur DHT11  dht 22 a l'extérieur
#define DS18B20_PIN 7 // Capteur DS18B20
#define LOADCELL_DOUT_PIN 0  // Data HX711
#define LOADCELL_SCK_PIN 1   // Clock HX711
#define BATTERY_SENSOR_PIN A1 // Mesure de la tension batterie

// Résistances du pont diviseur
const float R1 = 82.0;  // Résistance 82kΩ
const float R2 = 22.0;  // Résistance 22kΩ
const float Vref = 3.3; // Référence ADC
const int resolution = 1023; // Résolution ADC 12 bits
//const int resolution = 1023 ; 
// Initialisation des capteurs
DHT dht22(DHT22_PIN, DHT22);
DHT dht22_outside(DHT11_PIN, DHT22);
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
float batteryVoltage;
int batteryPercentage;
float calibration_factor = 30226.0;  // Facteur de calibration HX711
byte buffer[16]; // Stocke les valeurs à envoyer (2 octets * 8 valeurs)

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
  Serial.println("=== Démarrage du système ===");

  connection();
  
  dht22.begin();
  dht22_outside.begin();
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
}

////////////////////////
///      LOOP      ///
////////////////////////

void measureBattery() {
  Serial.println("=== Lecture de la tension batterie ===");
  int rawValue = analogRead(BATTERY_SENSOR_PIN);
  float Vmesure = (rawValue * Vref) / resolution; // Conversion en tension
  batteryVoltage = Vmesure * 1.28; // Calcul de la tension batterie
  batteryPercentage = getBatteryPercentage(batteryVoltage);

  Serial.print("Tension Batterie: ");
  Serial.print(batteryVoltage, 2);
  Serial.print("V - Charge: ");
  Serial.print(batteryPercentage);
  Serial.println("%");

  buildMessage(batteryPercentage, 14); // Stockage en 2 octets
}

// Fonction pour convertir la tension en pourcentage de charge
int getBatteryPercentage(float voltage) {
if (voltage >= 4.2) return 100;
  else if (voltage >= 4.12) return 95;   
  else if (voltage >= 4.1) return 90;
  else if (voltage >= 4.05) return 85;
  else if (voltage >= 4.0) return 80;
  else if (voltage >= 3.95) return 75;
  else if (voltage >= 3.9) return 70;
  else if (voltage >= 3.85) return 65;
  else if (voltage >= 3.8) return 60;
  else if (voltage >= 3.75) return 55;
  else if (voltage >= 3.7) return 50;
  else if (voltage >= 3.65) return 45;
  else if (voltage >= 3.6) return 40;
  else if (voltage >= 3.55) return 35;
  else if (voltage >= 3.5) return 30;
  else if (voltage >= 3.45) return 25;
  else if (voltage >= 3.4) return 20;
  else if (voltage >= 3.35) return 15;
  else if (voltage >= 3.3) return 10;
  else return 0;

 // return ((voltage - 3.3)/0.9)*100;
}

void measureDHTSensors() {
  Serial.println("=== Lecture des capteurs DHT ===");
  humDHT22 = dht22.readHumidity();
  tempDHT22 = dht22.readTemperature();
  humDHT11 = dht22_outside.readHumidity();
  tempDHT11 = dht22_outside.readTemperature();

  Serial.print("Température DHT22 : "); Serial.print(tempDHT22); Serial.println(" °C");
  buildMessage(tempDHT22, 0);
  buildMessage(humDHT22, 2);
  buildMessage(tempDHT11, 4);
  buildMessage(humDHT11, 6);
}

void measureDS18B20() {
  Serial.println("=== Lecture du capteur DS18B20 ===");
  ds18b20.requestTemperatures();
  tempDS18B20 = ds18b20.getTempCByIndex(0);
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
  poids = scale.get_units(10);
  Serial.print("Poids mesuré : "); Serial.print(poids, 2); Serial.println(" kg");
  buildMessage(poids, 12);
}

void buildMessage(float donnee_float, int index) {
  short donnee_short = (short)(donnee_float * 100);
  buffer[index] = donnee_short & 0xFF;
  buffer[index + 1] = (donnee_short >> 8) & 0xFF;
}

void sendMessage() {
  Serial.println("=== ENVOI DES DONNÉES LoRa ===");
  modem.beginPacket();
  modem.write(buffer, 16);
  modem.endPacket(true);
}



void loop() {
  // Vérifier si LoRa est toujours connecté en testant un envoi vide
  if (modem.getDataRate() == -1) {  // -1 signifie que LoRa est déconnecté
    Serial.println("Connexion LoRa perdue, reconnexion...");
    connection();
  }

  // Lecture des capteurs
  measureDHTSensors();
  measureDS18B20();
  measureLuminosity();
  measureWeight();
  measureBattery();

  // Envoi des données LoRa
  sendMessage();

  // Mise en veille des capteurs et du modem avant sommeil
  Wire.end();
  scale.power_down();
  modem.sleep();

  Serial.println("Passage en Deep Sleep...");
  LowPower.deepSleep(300000); 

  // Réveil : réactiver les capteurs
  Serial.println("Réveil !");
  Wire.begin();
  scale.power_up();
}



