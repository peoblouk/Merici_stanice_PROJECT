
#ifndef __PN532_H
#define __PN532_H

#include "stm8s.h"
#include "delay.h"

#define MAX_FRAME_LEN 64

/****************** MAKRA *******************************************/
// Definice SPI pinů
#define CHIP_SELECT_PORT GPIOC     //? SPI chip select port
#define CHIP_SELECT_PIN GPIO_PIN_4 //? SPI chip select pin

#define SPI_PORT GPIOC     //? SPI port
#define SPI_RST GPIO_PIN_3 //? SPI RST

#define SPI_SCK GPIO_PIN_5 //? SPI SCK

#define SPI_MOSI GPIO_PIN_6 //? SPI MOSI
#define SPI_MISO GPIO_PIN_7 //? SPI MISO

#define CS_H GPIO_WriteHigh(CHIP_SELECT_PORT, CHIP_SELECT_PIN) // Konec komunikace
#define CS_L GPIO_WriteLow(CHIP_SELECT_PORT, CHIP_SELECT_PIN)  // Začátek komunikace
/******************************************************************/

// PN532 protocol constants
#define PN532_PREAMBLE (0x00)
#define PN532_STARTCODE1 (0x00)
#define PN532_STARTCODE2 (0xFF)
#define PN532_POSTAMBLE (0x00)

#define PN532_FRAME_STARTCODE 0x00
#define PN532_FRAME_STARTCODE2 0xFF
#define PN532_HOST_TO_PN532 0xD4
#define PN532_PN532_TO_HOST 0xD5
#define PN532_ACK_WAIT_TIME 10
#define PN532_SPI_STATREAD 0x02
#define PN532_SPI_DATAWRITE 0x01
#define PN532_SPI_DATAREAD 0x03
#define PN532_SPI_READY 0x01
//

/**
 * @brief  Init SPI pins and start SPI communication
 * @retval None
 */
void pn532_init(void);

/**
 * @brief  Create list with PN532 start sequence
 * @param  frame List of data, which you want to send to PN532
 * @param  len Lenght of data to send
 * @retval List with buffer containing
 */
uint8_t pn532_send_frame(uint8_t *frame, uint8_t len);

/**
 * @brief  Send request to PN532 to read UID of tag
 * @param  data List of data, which you want to send to PN532
 * @param  len Lenght of data to send
 * @retval len of received data
 */
uint8_t pn532_read_data(uint8_t *data, uint8_t len2);

#endif /* __PN532_H */