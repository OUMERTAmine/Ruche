#include "HX711.h"

const int LOADCELL_DOUT_PIN = 6;
const int LOADCELL_SCK_PIN  = 7;
HX711 scale;

// Facteur de calibration déterminé pour un poids de référence de 78,8 kg
float calibration_factor = 30226.0;

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }  // Pour les cartes nécessitant l'attente de la connexion série
  Serial.println("=== Calibration de la balance ===");
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Tare automatique après 5 secondes pour permettre l'ouverture du Serial Monitor
  Serial.println("Tare automatique dans 5 secondes...");
  delay(5000);
  scale.tare();
  Serial.println("Tare effectuée. La balance est à 0 kg.");
  
  // Application directe du facteur de calibration
  scale.set_scale(calibration_factor);
  Serial.print("Facteur de calibration utilisé : ");
  Serial.println(calibration_factor);
}

void loop() {
  // Lecture continue du poids
  float poids = scale.get_units(10);
  Serial.print("Poids : ");
  Serial.print(poids, 2);
  Serial.println(" kg");
  delay(500);
}
