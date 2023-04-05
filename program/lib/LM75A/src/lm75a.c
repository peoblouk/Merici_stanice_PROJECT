/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 05-04-2023 - 23:36:08
 * @modify date 05-04-2023 - 23:36:22
 * @desc [LM75A I2C]
 */

#include "lm75a.h"

char dataBuffer[8];

void LM75A_Init(uint8_t address1, uint8_t address2, uint8_t address3)
{
    I2C_DeInit();

    // uint8_t _address1 = address1 << 1;
    // CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);

    I2C_Init(100000, address1 << 1, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    I2C_Init(100000, address2 << 1, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    I2C_Init(100000, address3 << 1, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq() / 1000000);
    I2C_Cmd(ENABLE);

    delay_ms(500);
}

// Read temperature data from LM75A
int16_t LM75A_getdata(uint8_t adress_of_lm)
{
    uint8_t data[2];

    uint8_t _address_of_lm = adress_of_lm << 1; // Přepočet na 7bit

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
    data[0] = I2C_ReceiveData();    // Ulož MSB data do proměnné
    I2C_AcknowledgeConfig(DISABLE); // Vypni zpětnou odezvu
    I2C_GenerateSTOP(ENABLE);       // Vygeneruj stop
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    data[1] = I2C_ReceiveData(); // Ulož LSB data do proměnné

    int16_t temp = ((data[0] << 8) | data[1]) >> 5; // převod teploty na int
    if (temp & 0x0400)
    {
        temp |= 0xF800;
    }
    return temp;
}