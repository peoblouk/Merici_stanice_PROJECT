/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 05-04-2023 - 23:36:08
 * @modify date 05-04-2023 - 23:36:22
 * @desc [LM75A I2C]
 */

#include "lm75a.h"
////////////////////////////////////////////////////////////////////
void LM75A_Init(uint8_t address1, uint8_t address2, uint8_t address3)
{
    I2C_DeInit();
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);

    I2C_Init(100000, address1 << 1, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    I2C_Init(100000, address2 << 1, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    I2C_Init(100000, address3 << 1, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    I2C_Cmd(ENABLE);
    delay_ms(30);
}
////////////////////////////////////////////////////////////////////
// Read temperature data from LM75A
void LM75A_ReadTemperature(uint8_t adress_of_lm, uint8_t *data)
{
    uint8_t _address_of_lm = adress_of_lm << 1; // Přepočet na 7bit
    int16_t temp;
    // uint8_t data[2];

    I2C_GenerateSTART(ENABLE); // Vygeneruj start
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;                                                  // Počkej na vlajku
    I2C_Send7bitAddress(_address_of_lm, I2C_DIRECTION_TX); // Pošli adresu slave
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(0x00); // Žádost o teplotu
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP(ENABLE); // Vygeneruj stop
    ///////////////////////////////////////////////////////////////////
    I2C_GenerateSTART(ENABLE); // Vygeneruj start
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(_address_of_lm, I2C_DIRECTION_RX);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    data[0] = I2C_ReceiveData();    // Ulož MSByte data do proměnné
    I2C_AcknowledgeConfig(DISABLE); // Vypni zpětnou odezvu
    I2C_GenerateSTOP(ENABLE);       // Vygeneruj stop
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    data[1] = I2C_ReceiveData(); // Ulož LSByte data do proměnné

    temp = ((data[0] << 8) | data[1]) >> 5; // převod teploty na int
    if (temp & 0x0400)
    {
        temp |= 0xF800;
    }

    data[0] = (temp / 10); // MSBytes
    data[1] = (temp % 10); // LSBytes

    // data[0] = msb;
    // data[1] = lsb;
}
////////////////////////////////////////////////////////////////////
/*
char LM75A_Temperature(uint8_t address)
{
    char temp_str[12];
    int temperatur = LM75A_ReadRegister(address);

    // lm75a_temp = (float)temperatur / 8.0;

    sprintf(temp_str, "%d.%d", temperatur, (temperatur / 8));

    return lm75a_temp;
}

 */
////
//! Example ((temp_hi << 8) | temp_lo) >> 5
// Decode temperature data

// msb_temp = (data_lm75a[0] << 3); // First eight bits contain Temperature
// lsb_temp = (data_lm75a[1] >> 5); // Another 3 bits contain Temperature in tens

// 1011 1000
// 0000 0011
// --------- OR
// 1011 1011
/////

// temp = ((data_lm75a[0] << 8) | data_lm75a[1]);
// temp >>= 5;        // ZahoĎ posledních 5 bitů (Rozlišení = 0.125 degrees Celsius)

// temp = uint16_t(((data[0] << 8) | data[1]));
// temp >>= 5; // discard 5 least significant bits (resolution = 0.125 degrees Celsius)
// if (temp & 0x0400)
// {                   // check if temperature is negative
//     temp |= 0xF800; // sign-extend temperature value
// }

/* // Store temperature data (MSB and LSB) in output buffer
// data[0] = ((temp >> 8) & 0xFF);
// data[1] = (temp & 0xFF); // (uint8_t)

int16_t temp = ((data[0] << 8) | data[1]) >> 5; // převod teploty na int
if (temp & 0x0400)
{
    temp |= 0xF800;
 */
