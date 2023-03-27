#ifndef RC522_H_
#define RC522_H_

#include "stm8s.h"

// RC522 commands
#define PCD_IDLE 0x00
#define PCD_AUTHENT 0x0E
#define PCD_RECEIVE 0x08
#define PCD_TRANSMIT 0x04
#define PCD_TRANSCEIVE 0x0C
#define PCD_RESETPHASE 0x0F
#define PCD_CALCCRC 0x03
#define PICC_REQIDL 0x26    // find the antenna area does not enter hibernation
#define PICC_SELECTTAG 0x93 // election card
#define PICC_ANTICOLL 0x93  // anti-collision

// RC522 registers
#define Reserved00 0x00
#define CommandReg 0x01
#define CommIEnReg 0x02
#define DivIEnReg 0x03
#define CommIrqReg 0x04
#define DivIrqReg 0x05
#define ErrorReg 0x06
#define Status1Reg 0x07
#define Status2Reg 0x08
#define FIFODataReg 0x09
#define FIFOLevelReg 0x0A
#define WaterLevelReg 0x0B
#define ControlReg 0x0C
#define BitFramingReg 0x0D
#define CollReg 0x0E
#define Reserved01 0x0F

// RC522 status codes
#define MI_OK 0
#define MI_NOTAGERR 1
#define MI_ERR 2

// RC522 card types
#define TAG_TYPE_UNKNOWN 0
#define PICC_TYPE_ISO_14443A 1
#define PICC_TYPE_ISO_14443B 2
#define TAG_TYPE_MIFARE_1K 3
#define PICC_TYPE_MIFARE_4K 4
#define TAG_TYPE_MIFARE_UL 5
#define TAG_TYPE_MIFARE_MINI 6

// Dummy byte
#define MFRC522_DUMMY 0x00

void rc522_init(void);
uint8_t rc522_read_register(uint8_t addr);
void rc522_write_register(uint8_t addr, uint8_t val);
void rc522_antenna_on(void);
void rc522_antenna_off(void);
uint8_t rc522_request(uint8_t req_mode, uint8_t *tag_type);
uint8_t rc522_send_command(uint8_t cmd, uint8_t *data, uint8_t data_len, uint8_t *response, uint16_t *response_len);
void rc522_set_register_bitmask(uint8_t addr, uint8_t mask);
void rc522_clear_register_bitmask(uint8_t addr, uint8_t mask);
uint8_t rc522_card_select(uint8_t *uid);
uint8_t rc522_request(uint8_t req_mode, uint8_t *tag_type);
uint8_t rc522_anticoll(uint8_t *ser_num);
void rc522_calculate_crc(uint8_t *data, uint8_t length, uint8_t *result);

#endif /* RC522_H_ */