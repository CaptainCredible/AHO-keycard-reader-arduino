// uses these libs
// MFRC522 by github community  version 1.4.10

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_1_PIN        10         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN        0          // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 1
#define NR_OF_READERS   1

byte ssPins[] = {SS_1_PIN, SS_2_PIN};

MFRC522 mfrc522;   // Create MFRC522 instance.

byte buzzerPin = 7;
/**
   Initialize.
**/

int greenLed = 8;
int redLed = 9;
bool lockState = true;
unsigned long lastTimer = 0;


void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  pinMode(greenLed,OUTPUT);
  pinMode(redLed,OUTPUT);
  setPins(lockState);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(SS_1_PIN, RST_PIN); // Init each MFRC522 card
  //mfrc522.PCD_DumpVersionToSerial();
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, LOW);
  beep(2);
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  beep(4);
}

/**
   Main loop.
*/



int userNumber = 4;
String users[2][4] = {
  {
  "D44F3F5B",
  "541451FF",
  "A4CA1054",
  "A4CA1055"
  },{
  "Karlheinz Stockhausen",
  "Hans Gerhard Meier",
  "Sploop",
  "Daniel Lacey-McDermott"
  }
};

void beep(byte numBeep){
  for(byte i = 0; i<numBeep; i++){
    digitalWrite(buzzerPin, LOW);
    delay(50);
    digitalWrite(buzzerPin, HIGH);
    delay(50);
  }
  
}

void setPins(bool isItLocked){
  if(isItLocked){
    digitalWrite(redLed,HIGH);
    digitalWrite(greenLed,LOW);
  } else {
    digitalWrite(redLed,LOW);
    digitalWrite(greenLed,HIGH);
  }
  
}

 long unsigned int lastCardTimer = 0;

void loop() {

    if (Serial.available()) {
    String inputString = Serial.readStringUntil('#');
    if (inputString == "I") {
      //Serial.println("IIIIIIII");
      lockState = false;
      setPins(lockState);
      lastTimer = millis();
    } else if (inputString == "O") {
      //Serial.println("OOOOOOOO");
      lockState = true;
      setPins(lockState);
      lastTimer = millis();
    }
  }

    //check for cards
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() && (millis()-lastCardTimer > 2000)) { 
      // maybe the problem is that i buffer reads when waiting here, so the check for lastCardTimer vs millis should be done i read the UID string to empty a buffer
      lastCardTimer = millis();
      String uidString = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        uidString += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        uidString += String(mfrc522.uid.uidByte[i], HEX);
        uidString.toUpperCase();
      }
      bool accessGranted = false;
      Serial.println(uidString); // for p5 sketch
      String thisUserString = "";
      for(int i = 0; i<userNumber; i++){
        if(uidString == users[0][i]){
          accessGranted = true;
          thisUserString = users[1][i];
        }
      }
      mfrc522.PICC_HaltA(); // what does this do ?
      mfrc522.PCD_StopCrypto1(); // what does this do ?
      beep(1);
    }

  if(millis()-lastTimer > 5000){
    if(lockState){
      Serial.println("OOOOOOOO");
    } else {
      Serial.println("IIIIIIII");
    }
    lastTimer = millis();
  }
}
