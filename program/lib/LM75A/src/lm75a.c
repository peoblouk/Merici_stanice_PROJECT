
#include "lm75a.h"

char dataBuffer[8];

void LM75A_Init(uint8_t address1, uint8_t address2, uint8_t address3)
{
    I2C_DeInit();

    // uint8_t _address1 = address1 << 1;
    // CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);

    I2C_Init(100000, address1, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    // I2C_Init(100000, address2, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    // I2C_Init(100000, address3, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    I2C_Cmd(ENABLE);

    delay_ms(500);
}

// uint16_t LM75A_getdata(uint8_t adress)
// {
//     while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
//         ;            // Počkej pokud je I2C zaneprázdněná
//     I2C_Cmd(ENABLE); // Vygeneruj start

//     while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
//         ;
//     I2C_Send7bitAddress(adress, I2C_DIRECTION_TX); // Odešli adresu slave

//     while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//         ; // Zkontroluj zda flag jestli není zaneprázdněný
//     // Read the first data
//     while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
//         ;
//     dataBuffer[0] = I2C_ReceiveData();

//     I2C_AcknowledgeConfig(DISABLE); // Disable ACK and generate stop condition
//     I2C_GenerateSTOP(ENABLE);       // Disable ACK and generate stop condition
//     while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
//         ;
//     dataBuffer[1] = I2C_ReceiveData(); // Ulož do proměnné
// }

// Read temperature data from LM75A
int16_t LM75A_getdata(uint8_t adress_of_lm)
{
    uint8_t data[2];

    // Send command to LM75A to read temperature
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(adress_of_lm, I2C_DIRECTION_TX);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(0x00);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP(ENABLE);

    // Read temperature data from LM75A
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(adress_of_lm, I2C_DIRECTION_RX);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    data[0] = I2C_ReceiveData();
    I2C_AcknowledgeConfig(DISABLE);
    I2C_GenerateSTOP(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    data[1] = I2C_ReceiveData();

    // Convert temperature data to signed integer
    int16_t temp = ((data[0] << 8) | data[1]) >> 5;
    if (temp & 0x0400)
    {
        temp |= 0xF800;
    }

    return temp;
}

float LM75A_GetTemperature(uint8_t address)
{
    LM75A_getdata(address);
    int temperature;
    temperature = 0.5 * ((dataBuffer[0] << 8 | dataBuffer[1]) >> 7);
    return temperature;
}