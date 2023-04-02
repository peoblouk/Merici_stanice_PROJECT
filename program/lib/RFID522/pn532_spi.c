#include "pn532_spi.h"

#define _SPI_STATREAD 0x02
#define _SPI_DATAWRITE 0x01
#define _SPI_DATAREAD 0x03
#define _SPI_READY 0x01

#define _SPI_TIMEOUT 10
// This indicates if the bits read/write should be reversed
#define _SPI_HARDWARE_LSB

/**************************************************************************
 * Reset
 **************************************************************************/
int PN532_Reset(void)
{
    GPIO_WriteHigh(SPI_PORT, SPI_RST);
    delay_ms(100);
    GPIO_WriteLow(SPI_PORT, SPI_RST);
    delay_ms(500);
    GPIO_WriteHigh(SPI_PORT, SPI_RST);
    delay_ms(100);
    return PN532_STATUS_OK;
}

void PN532_Init(void)
{
    SPI_DeInit();
    CS_L;
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_4, SPI_MODE_MASTER,
             SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_2EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX,
             SPI_NSS_SOFT, 0x10);
    SPI_Cmd(ENABLE);
    PN532_Reset();
    PN532_SPI_Wakeup();
}
/**************************************************************************
 * End: Reset
 **************************************************************************/
/**************************************************************************
 * SPI
 **************************************************************************/
uint8_t reverse_bit(uint8_t num)
{
    uint8_t result = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        result <<= 1;
        result += (num & 1);
        num >>= 1;
    }
    return result;
}

void spi_rw(uint8_t *data, uint8_t count)
{
    CS_L;
    delay_ms(1);
    char data[];
    for (uint8_t i = 0; i < count; i++)
    {
        data[i] = reverse_bit(data[i]);
    }

    data = SPI_ReceiveData();

    for (uint8_t i = 0; i < count; i++)
    {
        data[i] = reverse_bit(data[i]);
    }
    delay_ms(1);
    CS_H;
}

int PN532_SPI_ReadData(uint8_t *data, uint16_t count)
{
    uint8_t frame[count + 1];
    frame[0] = _SPI_DATAREAD;
    delay_ms(5);
    spi_rw(frame, count + 1);
    for (uint8_t i = 0; i < count; i++)
    {
        data[i] = frame[i + 1];
    }
    return PN532_STATUS_OK;
}

int PN532_SPI_WriteData(uint8_t *data, uint16_t count)
{
    uint8_t frame[count + 1];
    frame[0] = _SPI_DATAWRITE;
    for (uint8_t i = 0; i < count; i++)
    {
        frame[i + 1] = data[i];
    }
    spi_rw(frame, count + 1);
    return PN532_STATUS_OK;
}

bool PN532_SPI_WaitReady(uint32_t timeout)
{
    uint8_t status[] = {_SPI_STATREAD, 0x00};
    uint32_t tickstart = HAL_GetTick();
    while (HAL_GetTick() - tickstart < timeout)
    {
        delay_ms(10);
        spi_rw(status, sizeof(status));
        if (status[1] == _SPI_READY)
        {
            return true;
        }
        else
        {
            delay_ms(5);
        }
    }
    return false;
}

int PN532_SPI_Wakeup(void)
{
    // Send any special commands/data to wake up PN532
    uint8_t data[] = {0x00};
    delay_ms(1000);
    CS_L;
    delay_ms(2); // T_osc_start
    spi_rw(data, 1);
    delay_ms(1000);
    return PN532_STATUS_OK;
}

/* void PN532_SPI_Init()
{
    // init the pn532 functions
    pn532->reset = PN532_Reset;
    pn532->read_data = PN532_SPI_ReadData;
    pn532->write_data = PN532_SPI_WriteData;
    pn532->wait_ready = PN532_SPI_WaitReady;
    pn532->wakeup = PN532_SPI_Wakeup;
    pn532->log = PN532_Log;

    // hardware wakeup
    pn532->wakeup();
} */
/**************************************************************************
 * End: SPI
 **************************************************************************/