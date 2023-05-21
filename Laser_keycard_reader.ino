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

/**
   Initialize.
*/

int greenLed = 8;
int redLed = 9;

void setup() {

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(SS_1_PIN, RST_PIN); // Init each MFRC522 card
    //mfrc522.PCD_DumpVersionToSerial();
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

bool locked = true;
unsigned long lastTimer = 0;


void loop() {
    if (Serial.available()) {
    String inputString = Serial.readString();
    if (inputString == "I") {
      locked = false;
      lastTimer = millis();
    } else if (inputString == "O") {
      locked = true;
      lastTimer = millis();
    }
  }
    // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // Show some details of the PICC (that is: the tag/card)
      //Serial.print(F(": Card UID:"));
      //dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
      //Serial.println();
      String uidString = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        uidString += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        uidString += String(mfrc522.uid.uidByte[i], HEX);
        uidString.toUpperCase();
      }
      bool accessGranted = false;
      //Serial.print("uidString = ");
      Serial.println(uidString);
      String thisUserString = "";
      for(int i = 0; i<userNumber; i++){
        if(uidString == users[0][i]){
          accessGranted = true;
          thisUserString = users[1][i];
        }
      }
      
      if (accessGranted) {
        //Serial.println("ACCESS GRANTED");
        //Serial.print("%NAME%");
      } else {
        //Serial.println("ACCESS DENIED");
      }
      //Serial.println();
      mfrc522.PICC_HaltA();
      // Stop encryption on PCD
      mfrc522.PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC
  if(millis()-lastTimer > 2000){
    if(locked){
      Serial.println("OOOOOOOO");
    } else {
      Serial.println("IIIIIIII");
    }
    lastTimer = millis();
  }
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
