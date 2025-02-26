void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Définit la LED intégrée comme sortie
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // Allume la LED
  delay(1000);                      // Attend 1 seconde
  digitalWrite(LED_BUILTIN, LOW);   // Éteint la LED
  delay(1000);                      // Attend 1 seconde
}
