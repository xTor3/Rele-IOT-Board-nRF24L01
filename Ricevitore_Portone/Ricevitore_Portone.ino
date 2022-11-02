//Ricevitore Portone By xTor3

/*
PORTONE:
  Projector: Langman
    Open: 36  Ir:  83603535 (DOWN)
*/



//Librerie

#include <SPI.h>
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"

//Variabili Ricetrasmittente:

Nrf24l Mirf = Nrf24l(10, 9);      //(10, 9) in Caso di Utilizzo Con Arduino Nano RF
int dati;

//Variabili per AutoReset:

#define pinreset 3
unsigned long long t = 0;
unsigned long long delayreset = 600000;     //10 minuti consigliato.

//Variabili Per gli Output:

int releportone = 5; //Pin Rele Portone

bool a = 0;



void setup() {
  Serial.begin(9600); 
  delay(250);
  
  digitalWrite(pinreset, HIGH);     //Imposto il pin di autoreset ALTO, che corrrisponde allo stato di riposo
  pinMode(pinreset, OUTPUT);
  digitalWrite(pinreset, HIGH);

  pinMode(releportone, OUTPUT);
  digitalWrite(releportone, LOW);

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"LMNOP");   //Indirizzo scelto
  Mirf.payload = sizeof(dati); 
  Mirf.channel = 20;             
  Mirf.config();

  t = millis();                     //Salvo il valore di millis() subito dopo l'accensione
  
  Serial.println("Ricevitore Portone Pronto.");
}

void loop() {
  Mirf.ceLow();
  Mirf.configRegister(RF_CH, 20);
  Mirf.ceHi();
  
  if(Mirf.dataReady()){
    Mirf.getData((byte *) &dati);
    comandi(dati);                    //Mando alti rispettivi valori del vettore a, in base ai dati ricevuti.
    printout(dati);                   //Printo nella Seriale le azioni in base a ciò che ho ricevuto.
    cosafare();                       //Agisco in funzione dei dati ricevuti.
  }
  
  if((millis()-t) > delayreset){          //AutoReset automatico, con delay impostabile.
    digitalWrite(pinreset, LOW);
  }
  delay(100);
}



void cosafare(){
  if(a){                                     //Apertura Portone
    digitalWrite(releportone, HIGH);
    delay(1000);
    digitalWrite(releportone, LOW);
  }
  else{
    digitalWrite(releportone, LOW);;
  }
  a = 0;
}



void comandi(int l){
  switch(l){
    case(36):
      a = 1;
      break; 
    default:
      break;  
  }
}



void printout(int o){
  switch(o){
    case(36):
      Serial.println("Portone Aperto");
      break;
    default:
      Serial.println("Valore Sbagliato, Non faccio niente");
      break;  
  }
}



void debug_output(){
  digitalWrite(releportone, HIGH);
  delay(1000);
  digitalWrite(releportone, LOW);
  delay(1000);
}
