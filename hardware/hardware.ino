#include <SPI.h>
#include <MFRC522.h>
#include "conf.h"

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Predefined keys for dictionary attack
MFRC522::MIFARE_Key predefinedKeys[] = {
  {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
  {{0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}},
  {{0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7}},
  {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  // Add more keys as needed
};

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  Serial.println(F("Scan a MIFARE Classic card..."));
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("Card UID: "));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Iterate through each sector (0 to 15)
  for (int sector = 0; sector < 16; sector++) {
    readSector(sector);
  }

  // Halt PICC and stop encryption
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void readSector(int sector) {
  Serial.print(F("Sector "));
  Serial.print(sector);
  Serial.println(F(":"));

  // Iterate through each block in the sector (0 to 3)
  for (int block = 0; block < 4; block++) {
    readBlock(sector, block);
  }
  Serial.println(); // Split sector dumps for readability
}

void readBlock(int sector, int block) {
  byte readData[18];
  int absoluteBlock = sector * 4 + block;

  if (dictionaryAttack(absoluteBlock, readData)) {
    Serial.print(F("Block "));
    Serial.print(block);
    Serial.print(F(": "));

    for (byte i = 0; i < 16; i++) {
      Serial.print(readData[i] < 0x10 ? " 0" : " ");
      Serial.print(readData[i], HEX);
    }
    Serial.println();
  } else {
    Serial.print(F("Block "));
    Serial.print(block);
    Serial.println(F(": Failed to find the correct key"));
  }
}

bool dictionaryAttack(byte block, byte *buffer) {
  byte trailerBlock = (block / 4) * 4 + 3;
  byte size = 18;

  for (int i = 0; i < sizeof(predefinedKeys) / sizeof(predefinedKeys[0]); i++) {
    MFRC522::MIFARE_Key key = predefinedKeys[i];

    // Authenticate with the current key
    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status == MFRC522::STATUS_OK) {
      // If authentication is successful, try to read the block
      status = mfrc522.MIFARE_Read(block, buffer, &size);
      if (status == MFRC522::STATUS_OK) {
        // Key found, return true
        return true;
      }
    }
  }

  // If no key is found, return false
  return false;
}

// Function to print buffer in HEX format
void printBuffer(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}