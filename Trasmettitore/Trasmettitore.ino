//Trasmettitore Generale by xTor3

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

SARACINESCA:
  Projector: Langman
    Open: 33  Ir:  83617815 (UP)
    Close: 34  Ir:  83599455 (DX)
    Stop: 35  Ir: 83559165 (VOLUME-DOWN)

PORTONE:
  Projector: Langman
    Open: 36  Ir:  83603535 (DOWN)
*/



//Librerie

#include <IRremote.h>
#include <SPI.h>
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"



//Variabili Ricetrasmittente

Nrf24l Mirf = Nrf24l(9, 10);


//Variabili Ricevitore IR

int RECV_PIN = 6;  
IRrecv irrecv(RECV_PIN);
decode_results results;


//Variabili per AutoReset:

#define pinreset 3
unsigned long long t = 0;


//Variabili Globali:

//5 LUCI
int On1 = 22; 
int Off1 = 23;  
#define Ir1On 281563566
#define Ir1Off 281575806


int On2 = 24;  
int Off2 = 25;  
#define Ir2On 281592126
#define Ir2Off 281583966


int On3 = 26;  
int Off3 = 27;  
#define Ir3On 281588046
#define Ir3Off 281551326


int On4 = 28;  
int Off4 = 29;  
#define Ir4On 281593911
#define Ir4Off 281561271


int On5 = 30;  
int Off5 = 31;  
#define Ir5On 281542911
#define Ir5Off 281607936

//SARACINESCA
int OpenSar = 33;
int CloseSar = 34;
int StopSar = 35;
#define IrOpenSar 83617815 
#define IrCloseSar 83599455 
#define IrStopSar 83559165


//PORTONE
int PortoneInteger = 36;
#define IrPortoneInteger 83603535 



void setup() {
  Serial.begin(115200);
  delay(250);

  digitalWrite(pinreset, HIGH);     //Imposto il pin di autoreset ALTO, che corrrisponde allo stato di riposo.
  pinMode(pinreset, OUTPUT);
  digitalWrite(pinreset, HIGH);
  
  irrecv.enableIRIn(); 

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"LMNOQ");

  t = millis();                     //Salvo il valore di millis() subito dopo l'accensione

  Serial.println("Trasmettitore Pronto.");
}

void loop() {
  if (irrecv.decode(&results)){                   //Ricevo Segnali Ir
    Serial.println("");
    Serial.print("Valore Ricevuto: ");
    Serial.println(results.value);

    switch(results.value){                        //Vari Casi
      //5 Luci
      case(Ir1On):
        senddata(On1);
        printout(On1);
        break; 
      case(Ir1Off):
        senddata(Off1);
        printout(Off1);
        break;
      case(Ir2On):
        senddata(On2);
        printout(On2);
        break;
      case(Ir2Off):
        senddata(Off2);
        printout(Off2);
        break;
      case(Ir3On):
        senddata(On3);
        printout(On3);
        break; 
      case(Ir3Off):
        senddata(Off3);
        printout(Off3);
        break;
      case(Ir4On):
        senddata(On4);
        printout(On4);
        break;
      case(Ir4Off):
        senddata(Off4);
        printout(Off4);
        break;
      case(Ir5On):
        senddata(On5);
        printout(On5);
        break;
      case(Ir5Off):
        senddata(Off5);
        printout(Off5);
        break;

      //Saracinesca
      case(IrOpenSar):
        senddata(OpenSar);
        printout(OpenSar);
        break;
      case(IrCloseSar):
        senddata(CloseSar);
        printout(CloseSar);
        break;      
      case(IrStopSar):
        senddata(StopSar);
        printout(StopSar);
        break;
        
      //Portone
      case(IrPortoneInteger):
        senddata(PortoneInteger);
        printout(PortoneInteger);
        break;
        
      default:
        printout(255);
        break;
    }    
    irrecv.resume();
  }
  
  if((millis()-t) > 300000){
    digitalWrite(pinreset, LOW);
  }
  
  delay(100);
}



void senddata(int y){
  Mirf.payload = sizeof(y);
  Mirf.channel = 20;      
  Mirf.config(); 
  Mirf.setTADDR((byte *)"ABCDE");
  
  //sending
  Mirf.send((byte *)&y);
}



void printout(int x){
  switch(x){
    
    //5 Luci
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

    //Saracinesca
    case(33):
      Serial.println("Apertura Saracinesca");
      break;   
    case(34):
      Serial.println("Chiusura Saracinesca");
      break;
    case(35):
      Serial.println("Fermo la Saracinesca");
      break;

    //Portone
    case(36):
      Serial.println("Chiusura Portone");
      break;

    //Errori
    case(255):
      Serial.println("Valore Sbagliato, Non faccio niente");
      break;
      
    default:
      break;  
  }
}
