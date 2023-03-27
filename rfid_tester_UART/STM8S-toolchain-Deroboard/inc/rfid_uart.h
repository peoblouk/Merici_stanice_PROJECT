#ifndef RFID_H
#define RFID_H

#include "stm8s.h"
#include "delay.h"

//------------------MFRC522 register ---------------
#define COMMAND_WAIT 0x02
#define COMMAND_READBLOCK 0x03
#define COMMAND_WRITEBLOCK 0x04
#define MFRC522_HEADER 0xAB

#define STATUS_ERROR 0
#define STATUS_OK 1

#define MIFARE_KEYA 0x00
#define MIFARE_KEYB 0x01

// void RFID_write(char message[]);
// char RFID_read(void);

unsigned char ucAddr;
unsigned char ucValu;
uint8_t IncomingData;

void RFID_Begin(void);
unsigned char RFID_read(unsigned char Address);
void RFID_write(unsigned char Address, unsigned char value)

#endif