//Ricevitore Garage By xTor3

/*
SARACINESCA:
  Projector: Langman
    Open: 33  Ir:  83617815 (UP)
    Close: 34  Ir:  83599455 (DX)
    Stop: 35  Ir: 83559165 (VOLUME-DOWN)
*/



//Librerie:

#include <SPI.h>
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"


//Variabili Ricetrasmittente:

Nrf24l Mirf = Nrf24l(10, 9);      //(10, 9) in Caso di Utilizzo Con Arduino Nano RF
int dati;

//Variabili per AutoReset:

#define pinreset 4
unsigned long long t = 0;
unsigned long long delayreset = 600000;     //10 minuti consigliato.

//Variabili Per gli OUTPUT:

int relechiusura = 2; //Pin Rele Chiusura
int releapertura = 3; //Pin Rele Apertura

int tempoapertura = 5000; //Inserire Tempo Necessario All'Apertura
int tempochiusura = 5000; //Inserire Tempo Necessario Alla Chiusura

bool a[3] = {0, 0, 0};  //0 = Apri, 1 = Chiudi, 2 = Fermati
bool b = 0;

bool c[2] = {0, 0};

unsigned long long timer = 0;



void setup() {
  Serial.begin(9600);
  delay(250);
  
  digitalWrite(pinreset, HIGH);     //Imposto il pin di autoreset ALTO, che corrrisponde allo stato di riposo.
  pinMode(pinreset, OUTPUT);
  digitalWrite(pinreset, HIGH);

  pinMode(relechiusura, OUTPUT);
  pinMode(releapertura, OUTPUT);
  digitalWrite(relechiusura, LOW);
  digitalWrite(releapertura, LOW);
  
  //debug_output();
  
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"LMNOP");   //Indirizzo scelto
  Mirf.payload = sizeof(dati); 
  Mirf.channel = 20;             
  Mirf.config();

  t = millis();                     //Salvo il valore di millis() subito dopo l'accensione.
  
  Serial.println("Ricevitore Garage Pronto.");
}

void loop() {
  Mirf.ceLow();
  Mirf.configRegister(RF_CH, 20);
  Mirf.ceHi();
  
  if(Mirf.dataReady()){
    Mirf.getData((byte *) &dati);
    comandi(dati);                    //Mando alti rispettivi valori del vettore a, in base ai dati ricevuti.
    printout(dati);                   //Printo nella Seriale le azioni in base a ciò che ho ricevuto.
  }
  cosafare();                         //Agisco in funzione dei dati ricevuti.
  
  if(((millis()-t) > delayreset) && !a[0] && !a[1] && !a[2]){     //AutoReset automatico con delay impostabile, solo in condizioni di riposo.
    digitalWrite(pinreset, LOW);
  }
  delay(100);
}



void cosafare(){
  //Apertura Saracinesca                                
  if(a[0] && !a[2] && !c[1]){                   //Se ricevo il segnale di apertura, e non sono fermo, entro nella condizione if.                                  
    c[0] = 1;                                   //Interblocco.
    if(a[0] && a[1]){                           //Se ricevo il segnale di chiusura, mentre sto aprendo, spengo il relé di apertura e aspetto un delay di sicurezza.              
      a[0] = 0;
      a[1] = 1;
      digitalWrite(releapertura, LOW);
      delay(1000);
      c[0] = 0;
    }
    else if(millis() - timer <= tempoapertura){   //Se il tempo trascorso, è inferiore a quello di apertura, il relé resta acceso.
      digitalWrite(releapertura, HIGH);
    }
    else{
      digitalWrite(releapertura, LOW);
      a[0] = 0;
      a[1] = 0;
      c[0] = 0;
    }
  }
  //Chiusura Saracinesca
  else if(a[1] && !a[2] && !c[0]){              //Se ricevo il segnale di chiusura, e non sono fermo, entro nella condizione if.                                    
    c[1] = 1;                                   //Interblocco.
    if(a[0] && a[1]){                           //Se ricevo il segnale di apertura, mentre sto chiudendo, spengo il relé di chiusura e aspetto un delay di sicurezza.              
      a[1] = 0;
      a[0] = 1;
      digitalWrite(relechiusura, LOW);
      delay(1000);
      c[1] = 0;
    }
    else if(millis() - timer <= tempochiusura){   //Se il tempo trascorso, è inferiore a quello di chiusura, il relé resta acceso.
      digitalWrite(relechiusura, HIGH);
    }
    else{
      digitalWrite(relechiusura, LOW);
      a[1] = 0;
      a[0] = 0;
      c[1] = 0;
    }
  } 
  else{
    //Blocco Movimento
    if(a[2]){                                   //Se ricevo il comando di fermarmi, spengo i rele.
      digitalWrite(relechiusura, LOW); 
      digitalWrite(releapertura, LOW);
      a[0] = 0;
      a[1] = 0;
      c[0] = 0;
      c[1] = 0;
      b = 1;                                    
    }
    else if(b == 1 && (a[0] || a[1])){        //Se mentre sono in blocco ricevo un segnale di apertura o chiusura entro nel ciclo if.
      a[2] = 0;                               //Azzero la variabile di blocco movimento.
      b = 0;
    }   
  }
}



void comandi(int l){
  switch(l){
    case(33):
      a[0] = 1;
      timer = millis();
      break;
    case(34):
      a[1] = 1;
      timer = millis();
      break; 
    case(35):
      a[2] = 1;
      timer = millis();
    default:
      break;  
  }
}



void printout(int o){
  switch(o){
    case(33):
      Serial.println("Apro la Saracinesca");
      break;
    case(34):
      Serial.println("Chiudo la Saracinesca");
      break;
    case(35):
      Serial.println("Blocco il Movimento");
      break;
    default:
      Serial.println("Valore Sbagliato, Non faccio niente");
      break;  
  }
}



void debug_output(){
  digitalWrite(relechiusura, HIGH);
  delay(1000);
  digitalWrite(relechiusura, LOW);
  delay(1000);
  digitalWrite(releapertura, HIGH);
  delay(1000);
  digitalWrite(releapertura, LOW);
  delay(1000);
}
