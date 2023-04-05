/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 05-04-2023 - 15:30:49
 * @modify date 05-04-2023 - 23:36:03
 * @desc [LM75A I2C]
 */

#ifndef __LM75A_I2C_H
#define __LM75A_I2C_H

#include "stm8s.h"
#include "delay.h"

// LM75A registers
#define LM75A_TEMP_REG 0x00
#define LM75A_CONF_REG 0x01
#define LM75A_THYS_REG 0x02
#define LM75A_TOS_REG 0x03

// LM75A configuration bits
#define LM75A_CONF_SD 0x01
#define LM75A_CONF_OS 0x02
#define LM75A_CONF_COMP 0x04
#define LM75A_CONF_POL 0x08

void LM75A_Init(uint8_t address1, uint8_t address2, uint8_t address3);
int16_t LM75A_getdata(uint8_t adress_of_lm);

#endif