#include <MKRWAN.h>
#include <Wire.h>
#include "ArduinoLowPower.h"
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BH1750.h>
#include "HX711.h"
#include <FlashStorage.h>

////////////////////////
///     SETUP      ///
////////////////////////

LoRaModem modem;
#define SECRET_APP_EUI "0000000000000001"
#define SECRET_APP_KEY "8062D07A1C984423278A7D938C73E534"
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

#define DHT22_PIN 2
#define DHT11_PIN 3
#define DS18B20_PIN 7
#define LOADCELL_DOUT_PIN 0
#define LOADCELL_SCK_PIN 1
#define BATTERY_SENSOR_PIN A1

const float R1 = 82.0;
const float R2 = 22.0;
const float Vref = 3.3;
const int resolution = 1023;

DHT dht22(DHT22_PIN, DHT22);
DHT dht22_outside(DHT11_PIN, DHT22);
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);
BH1750 lightMeter;
HX711 scale;

float tempDHT22, humDHT22;
float tempDHT11, humDHT11;
float tempDS18B20;
float luminosite;
float poids;
float batteryVoltage;
int batteryPercentage;
float calibration_factor = 30226.0;
byte buffer[16];

// Stockage du tare HX711 en flash
FlashStorage(tareStorage, long);

void connection() {
  if (!modem.begin(EU868)) {
    Serial.println("Échec de l'initialisation LoRa !");
    while (1);
  }
  Serial.print("Version module : ");
  Serial.println(modem.version());
  Serial.print("Device EUI : ");
  Serial.println(modem.deviceEUI());

  if (!modem.joinOTAA(appEui, appKey)) {
    Serial.println("Échec de connexion à TTN !");
    while (1);
  }
  Serial.println("Connecté à TTN !");
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Démarrage du système ===");

  connection();

  dht22.begin();
  dht22_outside.begin();
  ds18b20.begin();

  Wire.begin();
  if (!lightMeter.begin()) {
    Serial.println("Erreur : BH1750 non détecté !");
    while (1);
  }

  Serial.println("Initialisation du HX711...");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  delay(5000);

  long tareOffset = tareStorage.read();
  if (tareOffset == 0 || tareOffset == -1L) {
    Serial.println("Première utilisation : calibration en cours...");
    scale.tare(); // Mettre la balance à vide pour cette étape !
    tareOffset = scale.get_offset();
    tareStorage.write(tareOffset);
    Serial.println("Tare calibré et sauvegardé !");
  } else {
    Serial.println("Tare trouvé en mémoire, utilisation directe.");
    scale.set_offset(tareOffset);
  }

  scale.set_scale(calibration_factor);
}

void measureBattery() {
  int rawValue = analogRead(BATTERY_SENSOR_PIN);
  float Vmesure = (rawValue * Vref) / resolution;
  batteryVoltage = Vmesure * 1.28;
  batteryPercentage = getBatteryPercentage(batteryVoltage);
  buildMessage(batteryPercentage, 14);
}

int getBatteryPercentage(float voltage) {
  if (voltage >= 4.2) return 100;
  else if (voltage >= 4.1) return 95;
  else if (voltage >= 4.05) return 90;
  else if (voltage >= 4.0) return 85;
  else if (voltage >= 3.95) return 80;
  else if (voltage >= 3.9) return 75;
  else if (voltage >= 3.85) return 70;
  else if (voltage >= 3.8) return 65;
  else if (voltage >= 3.75) return 60;
  else if (voltage >= 3.7) return 55;
  else if (voltage >= 3.65) return 50;
  else if (voltage >= 3.6) return 45;
  else if (voltage >= 3.55) return 40;
  else if (voltage >= 3.5) return 35;
  else if (voltage >= 3.45) return 30;
  else if (voltage >= 3.4) return 20;
  else if (voltage >= 3.35) return 10;
  else if (voltage >= 3.3) return 5;
  else return 0;
}

void measureDHTSensors() {
  humDHT22 = dht22.readHumidity();
  tempDHT22 = dht22.readTemperature();
  humDHT11 = dht22_outside.readHumidity();
  tempDHT11 = dht22_outside.readTemperature();
  buildMessage(tempDHT22, 0);
  buildMessage(humDHT22, 2);
  buildMessage(tempDHT11, 4);
  buildMessage(humDHT11, 6);
}

void measureDS18B20() {
  ds18b20.requestTemperatures();
  tempDS18B20 = ds18b20.getTempCByIndex(0);
  buildMessage(tempDS18B20, 8);
}

void measureLuminosity() {
  luminosite = lightMeter.readLightLevel();
  buildMessage(luminosite, 10);
}

void measureWeight() {
  poids = scale.get_units(10);
  buildMessage(poids, 12);
}

void buildMessage(float donnee_float, int index) {
  short donnee_short = (short)(donnee_float * 100);
  buffer[index] = donnee_short & 0xFF;
  buffer[index + 1] = (donnee_short >> 8) & 0xFF;
}

bool sendMessageSafe() {
  modem.beginPacket();
  modem.write(buffer, 16);
  int err = modem.endPacket(true);
  return err > 0;
}

void loop() {
  if (modem.getDataRate() == -1) {
    Serial.println("Connexion LoRa perdue, reconnexion...");
    connection();
  }

  measureDHTSensors();
  measureDS18B20();
  measureLuminosity();
  measureWeight();
  measureBattery();

  if (!sendMessageSafe()) {
    Serial.println("Envoi LoRa échoué !");
    connection(); 
    connection();
  } else {
    Serial.println("Données envoyées !");
  }

  Wire.end();
  scale.power_down();
  modem.sleep();

  Serial.println("Deep Sleep 10 minutes...");
  LowPower.deepSleep(600000); 

  Serial.println("Réveil !");
  Wire.begin();
  scale.power_up();
}
