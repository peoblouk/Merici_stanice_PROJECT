
#include "lm75a.h"

char dataBuffer[8];

void LM75A_Init(uint8_t address1, uint8_t adress2, uint8_t adress3)
{
    delay_ms(1000);
    I2C_Init(100000, address1, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    // I2C_Init(100000, address2, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    // I2C_Init(100000, address3, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    I2C_Cmd(ENABLE);
}

uint16_t LM75A_getdata(uint8_t adress)
{
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
        ;            // Počkej pokud je I2C zaneprázdněná
    I2C_Cmd(ENABLE); // Vygeneruj start
    while (!I2C_GetFlagStatus(I2C_FLAG_SB))
        ;
    I2C_Send7bitAddress(adress, I2C_DIRECTION_TX); // Odešli adresu slave

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ; // Zkontroluj zda flag jestli není zaneprázdněný
    // Read the first data
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    dataBuffer[0] = I2C_ReceiveData();

    I2C_AcknowledgeConfig(DISABLE); // Disable ACK and generate stop condition
    I2C_GenerateSTOP(ENABLE);       // Disable ACK and generate stop condition
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    dataBuffer[1] = I2C_ReceiveData(); // Ulož do proměnné
}

extern float get_temperature_c(uint8_t address)
{
    LM75A_getdata(address);
    int temperature;
    temperature = 0.5 * ((dataBuffer[0] << 8 | dataBuffer[1]) >> 7);
    return temperature;
}