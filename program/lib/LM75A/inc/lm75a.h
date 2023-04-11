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

int msb, lsb;

// LM75A registers
#define LM75A_TEMP_REG 0x00 // Temperature register - (Read only) two 8-bit data
#define LM75A_CONF_REG 0x01 // Configuration register - (Read / Write) single 8-bit, default =0
#define LM75A_THYS_REG 0x02 // Hysteresis register - (Read / Write) two 8-bit, default = 75 °C
#define LM75A_TOS_REG 0x03  // Overtemperature shutdown threshold register (Read / Write) two 8-bit, default = 80 °C

// LM75A configuration bits
#define LM75A_CONF_SD 0x01
#define LM75A_CONF_OS 0x02
#define LM75A_CONF_COMP 0x04
#define LM75A_CONF_POL 0x08

///////////////////// PUBLIC FUNCTIONS //////////////////////////////
/**
 * @brief  Inicializace teploměrů.
 * @param  address1 : Adresa prvního slave zařízení.
 * @param  address2 : Adresa druhého slave zařízení.
 * @param  address3 : Adresa třetího slave zařízení.
 * @retval None
 */
void LM75A_Init(uint8_t address1, uint8_t address2, uint8_t address3);
////////////////////////////////////////////////////////////////////
///////////////////// PRIVATE FUNCTIONS ////////////////////////////
/**
 * @brief  I2C čtení z registrů a hrubé zpracování teploty.
 * @param  adress_of_lm : Adresa slave zařízení, pro které jde požadavek.
 * @param  msb_ : Proměnná, kde hodláme uložit hodnotu z čtení MSB.
 * @param  lsb_: Proměnná, kde hodláme uložit hodnotu z čtení LSB.
 * @retval None
 */
void LM75A_ReadTemperature(uint8_t adress_of_lm, uint8_t *data);
////////////////////////////////////////////////////////////////////

/**
 * @brief  Převod teploty na float.
 * @param  address : Adresa slave zařízení, pro které jde požadavek.
 * @retval Float teplota
 */
// int LM75A_Temperature(uint8_t address); // Decode temperature
#endif