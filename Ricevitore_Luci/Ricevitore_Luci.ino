//Ricevitore 5 Luci By xTor3

/* 
5 LUCI:
  Projector: Eiki
    Codice Relé 1:
      On: 22  Ir:  281563566 (OK)
      Off: 23  Ir:  281575806 (DX)
  
    Codice Relé 2:
      On: 24  Ir:  281592126 (SX)
      Off: 25  Ir:  281583966 (DOWN)
  
    Codice Relé 3:
      On: 26  Ir:  281588046 (NO-VOLUME)
      Off: 27  Ir:  281551326 (MENU')
  
    Codice Relé 4:
      On: 28  Ir:  281593911 (VOLUME-DOWN)
      Off: 29  Ir:  281561271 (VOLUME-UP)  
      
    Codice Relé 5:
      On: 30  Ir:  281542911 (TURN-BACK)
      Off: 31  Ir:  281607936 (ZOOM-OUT)
*/



//Librerie:

#include <SPI.h>
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"



//Variabili Ricetrasmittente:

Nrf24l Mirf = Nrf24l(9, 10);
int value;


//Variabili per AutoReset:

#define pinreset 7
#define delayreset 300000     //10 Minuti: 600000  -  5 Minuti: 300000.
unsigned long t = 0;


//Variabili per gli OUTPUT:

int pulsante[5] = {A0, A1, A2, A3, A4};
int rele[5] = {2, 3, 4, 5, 6};

bool r[5] = {0, 0, 0, 0, 0};

bool a[5] = {0, 0, 0, 0, 0};      //Comandi ON
bool a2[5] = {0, 0, 0, 0, 0};     //Comandi OFF

bool x[5] = {0, 0, 0, 0, 0};
bool y[5] = {0, 0, 0, 0, 0};
bool z[5] = {0, 0, 0, 0, 0};
bool q[5] = {0, 0, 0, 0, 0};


//Other:

unsigned long delay_print = 0;



void setup() {
  Serial.begin(115200);
  delay(250);

  digitalWrite(pinreset, HIGH);     //Imposto il pin di autoreset ALTO, che corrrisponde allo stato di riposo.
  pinMode(pinreset, OUTPUT);
  digitalWrite(pinreset, HIGH);

  for(int i = 0; i < 5; i++){
    pinMode(pulsante[i], INPUT); 
    pinMode(rele[i], OUTPUT);
    digitalWrite(rele[i], LOW);
  }

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"ABCDE"); 
  Mirf.payload = sizeof(value);
  Mirf.channel = 20;             
  Mirf.config();

  t = millis();                     //Salvo il valore di millis() subito dopo l'accensione.
  delay_print = millis();
  
  Serial.print("Ricevitore Luci Pronto.");
  Serial.println("");
}

void loop() {

  Mirf.ceLow();
  Mirf.configRegister(RF_CH, 20);  //switch channel 10
  Mirf.ceHi();
 
  if(Mirf.dataReady()){
    Mirf.getData((byte *) &value);
    comandi(value);                    //Mando alti rispettivi valori dei vettori a e a2 in base ai dati ricevuti.
    printout(value);                   //Printo nella Seriale le azioni in base a ciò che ho ricevuto.
  }
  
  letture();                          //Leggo Lo stato degli interruttori.
  cosafare();
  azionerele();
  
  if((millis() - delay_print) > 1000){
    Serial.println("");
    printstatorele();
    delay_print = millis();
  }
  
  if(((millis()-t) > delayreset) && !r[0] && !r[1] && !r[2] && !r[3] && !r[4]) digitalWrite(pinreset, LOW);          //AutoReset automatico con delay impostabile, solo in condizioni di riposo. 

  delay(100);
}



void printstatorele(){
  for(int i = 0; i < 5; i++){
    Serial.print("Relé: ");
    Serial.print(i+1);
    Serial.print("  Stato: ");
    if(r[i]) Serial.println("Acceso");
    else Serial.println("Spento");
    Serial.println("");
  }
}



void letture(){
  for(int i = 0; i < 5; i++){                   //Ripeto le letture n volte, pari al numero degli output.    
    if(analogRead(pulsante[i]) > 1000){         //Se il pulsante è chiuso, registro il valore nel vettore x.
      x[i] = 1;
    }
    else{
      y[i] = 1;                                 //Se il pulsante è aperto, registro il valore nel vettore y.
    }
  
    if(a[i]){                                   //Se ricevo dal trasmettitore il segnale di accensione, registro il valore nel vettore z.
      z[i] = 1;
    }
  
    if(a2[i]){                                  //Se ricevo dal trasmettitore il segnale di chiusura, registro il valore nel vettore q.
      q[i] = 1;
    }
  }
  for(int i = 0; i < 5; i++){                   //Dopo le letture, cancello i valori assegnati alle variabili in cui vengono registrati i valori ricevuti dal trasmettitore.
    a[i] = 0;
    a2[i] = 0;
  }
}



void cosafare(){
  for(int i = 0; i < 5; i++){                   //Ripeto le letture n volte, pari al numero degli output.
    if(x[i] == 1 && y[i] == 1){                 //Se l'interruttore i ha cambiato stato, cambio lo stato del rispettivo rele i. 
      r[i] = !r[i];
      x[i] = 0;
      y[i] = 0;
    }
    else{
      if(z[i]){                                 //Se ricevo dal trasmettiore il segnale di accensione, eseguo l'azione di accensione del rispettivo rele.
        r[i] = 1;  
        z[i] = 0;   
      }
      else if(q[i]){                            //Viceversa se ricevo il segnale di spegnere, agisco di conseguenza.
        r[i] = 0;
        q[i] = 0;
      }
    }
  }
}



void azionerele(){
  for(int i = 0; i < 5; i++){
    digitalWrite(rele[i], r[i]);                //Dopo aver effettuato le letture, e aver deciso cosa fare, imposto ai rele lo stato scelto dagli algoritmi precedenti.
  }
}



void comandi(int l){
  switch(l){
    case(22):
      a[0] = 1;
      break;
    case(23):
      a2[0] = 1;
      break;
    case(24):
      a[1] = 1;
      break;
    case(25):
      a2[1] = 1;
      break;
    case(26):
      a[2] = 1;
      break; 
    case(27):
      a2[2] = 1;
      break;
    case(28):
      a[3] = 1;
      break;
    case(29):
      a2[3] = 1;
      break;
    case(30):
      a[4] = 1;
      break;
    case(31):
      a2[4] = 1;
      break;
    default:
      break;  
  }
}



void printout(int x){
  Serial.println("");
  Serial.print("E' stato ricevuto: ");
  Serial.println(x);
  switch(x){
    case(22):
      Serial.println("Accensione relé 1");
      break;
    case(23):
      Serial.println("Spegnimento relé 1");
      break;
    case(24):
      Serial.println("Accensione relé 2");
      break;
    case(25):
      Serial.println("Spegnimento relé 2");
      break;
    case(26):
      Serial.println("Accensione relé 3");
      break; 
    case(27):
      Serial.println("Spegnimento relé 3");
      break;
    case(28):
      Serial.println("Accensione relé 4");
      break;
    case(29):
      Serial.println("Spegnimento relé 4");
      break;
    case(30):
      Serial.println("Accensione relé 5");
      break;
    case(31):
      Serial.println("Spegnimento relé 5");
      break;
    default:
      Serial.println("Valore Sbagliato, Non faccio niente");
      break;  
  }
}
