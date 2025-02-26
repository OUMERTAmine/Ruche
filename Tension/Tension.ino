const int sensorPin = A1;   // Broche AIN[10] (PB02 / A1 / D16)
const float R1 = 82.0;      // Résistance 82kΩ
const float R2 = 22.0;      // Résistance 22kΩ
const float Vref = 3.3;     // Tension de référence ADC (3.3V)
const int resolution = 4095; // Résolution ADC 12 bits (0 - 4095)

void setup() {
  Serial.begin(115200); // Initialisation du moniteur série
  while (!Serial);      // Attendre l’ouverture du port série
}

void loop() {
  int rawValue = analogRead(sensorPin); // Lire la valeur brute de l'ADC
  float Vmesure = (rawValue * Vref) / resolution; // Conversion en tension
  float Vbatt = Vmesure * ((R1 + R2) / R2); // Calcul de la tension batterie
  
  // Affichage sur le moniteur série
  Serial.print("Valeur ADC: ");
  Serial.print(rawValue);
  Serial.print(" - Tension mesurée: ");
  Serial.print(Vmesure, 3);
  Serial.print("V - Tension batterie: ");
  Serial.print(Vbatt, 3);
  Serial.println("V");
  
  delay(1000); // Pause de 1 seconde
}
