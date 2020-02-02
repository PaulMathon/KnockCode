#include <Wire.h>
#include "rgb_lcd.h"
#include "Ultrasonic.h"

// Ecran LCD
rgb_lcd lcd;
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// Capteur ultrasonic
Ultrasonic ultrasonic(6);

// Capteur de vibration
const int coupFaible = 10; // Intensité min des coups
int nombreDeCoups = 0;
int coup;
int tcoup=0;
int codeChoisi = 4; // nombre de coups permettant d'ouvrir la porte

// LEDS
const int ledPinR=4;
const int ledPinG=7;

void showLCDMsg(String msg);

void setup() 
{   
    //LCD
    // Initialisation de l'écran (nombre total de lignes et de colonnes)
    lcd.begin(16, 2);
    // Initialisation des couleurs :
    lcd.setRGB(colorR, colorG, colorB);

    // Message de test
    lcd.print("hello, world!");
    delay(1000);
    
    Serial.begin(9600);

    // Initialisation des LEDS
    pinMode(ledPinR,OUTPUT);
    pinMode(ledPinG,OUTPUT);
}

void loop() {   
  //DISTANCE
  long RangeInCentimeters;
  // Récupération de la valeur du capteur de distance
  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  Serial.print("Distance :");
  Serial.print(RangeInCentimeters);// affichage sur le moniteur série
  Serial.println(" cm");
  
  // si porte fermée, on peut compter le nombre de toc toc
  if (RangeInCentimeters<5 || RangeInCentimeters>519){
    Serial.println("La porte est fermée");
    digitalWrite(ledPinR,HIGH); // Led Rouge s'allume
    digitalWrite(ledPinG,LOW); // Led verte éteinte
  
    showLCDMsg("Porte fermee.");
  
    //Lecture du capteur de la vibration
    coup = digitalRead(0);
    Serial.print("State vibration :");
    Serial.println(coup);
  
    //s'il détecte une première vibration
    if(coup) {
      Serial.println("Vous avez dix seconde pour entrer le bon nombre de frappe");
      //On relève le temps 
      tcoup=millis();
      while (millis()<(tcoup+10000)){
        coup = digitalRead(0);
        if (coup){
          coup = 0;
          delay(200);
          if (coup==false){
            nombreDeCoups++;
            Serial.print("Nombre de coups : ");
            Serial.println(nombreDeCoups);
            if (nombreDeCoups == codeChoisi) {
              while (RangeInCentimeters<5 || RangeInCentimeters>519){
                RangeInCentimeters = ultrasonic.MeasureInCentimeters();
                digitalWrite(ledPinG,HIGH); // LED Verte s'allume
                digitalWrite(ledPinR,LOW); // LED Rouge s'éteint
                Serial.println("Vous pouvez ouvrir la porte");
                Serial.println(RangeInCentimeters);
                //LCD
                // Affichage :
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Code Bon, porte"); 
                lcd.setCursor(0, 1);
                lcd.print("ouverte.");
              }
              Serial.println("Vous avez ouvert la porte");
            }
          }
          
        }
      }
      Serial.println("Temps écoulé");
     
      
      nombreDeCoups=0;
    }     
  }else{   //La porte est ouverte, tout va bien 
    Serial.println("La porte est ouverte");
    
    digitalWrite(ledPinR,LOW); // Led Rouge est éteinte
    digitalWrite(ledPinG,HIGH); // Led verte s'allume
    
    showLCDMsg("La porte est ouverte");

    nombreDeCoups=0;

  }
}

void showLCDMsg(String msg){
  // Efface le message précédent
  lcd.clear();
  // Positionnement du curseur sur l'écran
  lcd.setCursor(0, 1);
  // Affichage :
  lcd.print(msg);
}

//La porte est-elle fermée?
//Si oui
    //Détecte un coup
    //Compte le nombre de coup : dans un intervalle de temps égal à 1.5s x le nb de coups paramétrés
    //Si le nombre de coups est bon 
        //On allume la LED = ouverture de la porte
        //Msg LCD : Mot de passe valide 
//Si non 
    //Si on détecte un coup
        //Envoi d'un msg d'erreur LCD : "La porte est déjà ouverte"
