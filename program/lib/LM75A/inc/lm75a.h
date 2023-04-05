/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 05-04-2023 - 15:30:49
 * @modify date 05-04-2023 - 15:30:49
 * @desc [description]
 */

#ifndef __LM75A_I2C_H
#define __LM75A_I2C_H

#include "stm8s"

void LM75A_Init(uint8_t address1, uint8_t adress2, uint8_t adress3);
uint16_t LM75A_getdata(uint8_t adress);
extern float get_temperature_c(uint8_t address);

#endif