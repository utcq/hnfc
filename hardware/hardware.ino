#include "conf.h"
#include <MFRC522.h>

MFRC522 mfrc522(SDA, RST);
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Initializing RFID reader");
  mfrc522.PCD_Init();
  Serial.println("RFID reader initialized");
}

void loop() {
  /*memset(key.keyByte, 0xFF, 6);
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println("Card found");
  Serial.print("UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));*/
  Serial.println("Card found");
}

