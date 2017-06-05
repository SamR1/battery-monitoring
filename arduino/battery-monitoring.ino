#include "MAX17043.h"
#include "Wire.h"

MAX17043 batteryMonitor;

float zero_senseur;
int PIN_ACS712 = A0;

// Obtient la valeur du senseur de courant ACS712
//
// Effectue plusieurs lecture et calcule la moyenne pour pondérer
// la valeur obtenue.
float valeurACS712( int pin ){
  int valeur;
  float moyenne = 0;

  int nbr_lectures = 50;
  for( int i = 0; i < nbr_lectures; i++ ){
      valeur = analogRead( pin );
      moyenne = moyenne + float(valeur);
  }
  moyenne = moyenne / float(nbr_lectures);
  return moyenne;
}

void setup(){
  // calibration du senseur  (SANS COURANT)
  zero_senseur = valeurACS712( PIN_ACS712 );

  Wire.begin();
  Serial.begin( 9600 );

  // fuel gauge init
  batteryMonitor.reset();
  batteryMonitor.quickStart();

}

float courant;
float courant_efficace;
float tension_efficace = 3.7; // tension efficace du réseau electrique
float puissance_efficace;
float ACS712_RAPPORT = 100; // nbr de millivolts par ampère

void loop(){
  float valeur_senseur = valeurACS712( PIN_ACS712 );
  // L'amplitude en courant est ici retournée en mA
  // plus confortable pour les calculs
  courant = (float)(valeur_senseur-zero_senseur)/1024*5/ACS712_RAPPORT*100000;
  // Courant efficace en mA
  courant_efficace = courant; // / 1.414;  divisé par racine de 2

  // Calcul de la puissance.
  //    On divise par 1000 pour transformer les mA en Ampère
  puissance_efficace = (courant_efficace * tension_efficace/1000);

  // fuel gauge : lecture voltage et pourcentage de charge
  float cellVoltage = batteryMonitor.getVCell();
  float stateOfCharge = batteryMonitor.getSoC();

  Serial.print(cellVoltage, 4);
  Serial.print("|");
  Serial.print(stateOfCharge);
  Serial.print("|");
  Serial.println(puissance_efficace);

  delay( 1000 ); // attendre une seconde
}
