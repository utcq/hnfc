#include <SPI.h>
#include <MFRC522.h>
#include "conf.h"

#define PRESENCE() if (!isPresent()) {return;}
#define WAIT() while (Serial.available() <= 0) { PRESENCE(); }

MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key predefinedKeys[] = {
  {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
  {{0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}},
  {{0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7}},
  {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
};

void setup() {
  Serial.begin(BAUD_RATE);
  SPI.begin();
  mfrc522.PCD_Init();
}

bool isPresent() {
  return mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial();
}

void read_cmd() {
  WAIT();
  byte sector = Serial.read();

  WAIT();
  byte block = Serial.read();

  readBlock(sector, block);
}

void parse_command() {
  byte cmd = Serial.read();

  switch (cmd) {
    case COMM_FULL_READ: {
      for (int sector = 0; sector < 16; sector++) {
        readSector(sector);
      }
      break;
    }

    case COMM_READ:
      read_cmd();
      break;
    
    default: {
      break;
    }
  }
}

void loop() {
  PRESENCE();
  delay(1500);

  Serial.write("\x60\x10", 2);
  Serial.write(mfrc522.uid.size);
  Serial.write(mfrc522.uid.uidByte, mfrc522.uid.size);

  WAIT();
  byte prefix = Serial.read();
  switch (prefix) {
    case COMM_PREFIX: {
      parse_command();
      break;
    }
    default: {
      break;
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void readSector(int sector) {
  for (int block = 0; block < 4; block++) {
    readBlock(sector, block);
  }
}

void readBlock(int sector, int block) {
  byte readData[18];
  int absoluteBlock = sector * 4 + block;

  if (dictionaryAttack(absoluteBlock, readData)) {
    for (byte i = 0; i < 16; i++) {
      Serial.print(readData[i] < 0x10 ? " 0" : " ");
      Serial.print(readData[i], HEX);
    }
  } else {
    Serial.print("-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --");
  }
  Serial.println();
}

bool dictionaryAttack(byte block, byte *buffer) {
  byte trailerBlock = (block / 4) * 4 + 3;
  byte size = 18;

  for (int i = 0; i < sizeof(predefinedKeys) / sizeof(predefinedKeys[0]); i++) {
    MFRC522::MIFARE_Key key = predefinedKeys[i];

    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status == MFRC522::STATUS_OK) {
      status = mfrc522.MIFARE_Read(block, buffer, &size);
      if (status == MFRC522::STATUS_OK) {
        return true;
      }
    }
  }

  return false;
}

void printBuffer(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}