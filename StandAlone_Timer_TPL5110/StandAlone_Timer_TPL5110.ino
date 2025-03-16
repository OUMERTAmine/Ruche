#define DONE_PIN 5  // Broche MKRWAN1310 connectée à DONE du TPL5110
#define DRV_PIN 4   // Broche MKRWAN1310 connectée à DRV du TPL5110

void setup() {
  pinMode(DONE_PIN, OUTPUT);
  digitalWrite(DONE_PIN, LOW); // Assure que le timer fonctionne au démarrage

  pinMode(DRV_PIN, INPUT); // Met DRV en lecture

  Serial.begin(115200);
}

void loop() {
  Serial.println("Démarrage TPL5110");
  
  delay(5000);  // Délai initial avant d'envoyer DONE

  digitalWrite(DONE_PIN, HIGH); // Envoyer un signal DONE pour couper la LED
  delay(1);
  digitalWrite(DONE_PIN, LOW); // Remettre le signal à l'état bas
  delay(1);

  Serial.println("ETEINT");

  // 🔍 Attente que DRV passe à 1 avec affichage de son état
  while (digitalRead(DRV_PIN) == LOW) {
    
  }

  Serial.println("ALLUME, redémarrage du cycle !");
}
