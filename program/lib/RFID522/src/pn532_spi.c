#include "pn532_spi.h"

#define _SPI_STATREAD 0x02
#define _SPI_DATAWRITE 0x01
#define _SPI_DATAREAD 0x03
#define _SPI_READY 0x01

#define _SPI_TIMEOUT 10
// This indicates if the bits read/write should be reversed
#define _SPI_HARDWARE_LSB

/////
/**
 * @brief: Read a response frame from the PN532 of at most length bytes in size.
 *     Note that less than length bytes might be returned!
 * @retval: Returns frame length or -1 if there is an error parsing the frame.
 */
int PN532_ReadFrame(PN532 *pn532, uint8_t *response, uint16_t length)
{
    uint8_t buff[PN532_FRAME_MAX_LENGTH + 7];
    uint8_t checksum = 0;
    // Read frame with expected length of data.
    PN532_SPI_ReadData(buff, length + 7);
    // Swallow all the 0x00 values that preceed 0xFF.
    uint8_t offset = 0;
    while (buff[offset] == 0x00)
    {
        offset += 1;
        if (offset >= length + 8)
        {
            // pn532->log("Response frame preamble does not contain 0x00FF!");
            return PN532_STATUS_ERROR;
        }
    }
    if (buff[offset] != 0xFF)
    {
        // pn532->log("Response frame preamble does not contain 0x00FF!");
        return PN532_STATUS_ERROR;
    }
    offset += 1;
    if (offset >= length + 8)
    {
        // pn532->log("Response contains no data!");
        return PN532_STATUS_ERROR;
    }
    // Check length & length checksum match.
    uint8_t frame_len = buff[offset];
    if (((frame_len + buff[offset + 1]) & 0xFF) != 0)
    {
        // pn532->log("Response length checksum did not match length!");
        return PN532_STATUS_ERROR;
    }
    // Check frame checksum value matches bytes.
    for (uint8_t i = 0; i < frame_len + 1; i++)
    {
        checksum += buff[offset + 2 + i];
    }
    checksum &= 0xFF;
    if (checksum != 0)
    {
        // pn532->log("Response checksum did not match expected checksum");
        return PN532_STATUS_ERROR;
    }
    // Return frame data.
    for (uint8_t i = 0; i < frame_len; i++)
    {
        response[i] = buff[offset + 2 + i];
    }
    return frame_len;
}
/////
int PN532_CallFunction(
    uint8_t command,
    uint8_t *response,
    uint16_t response_length,
    uint8_t *params,
    uint16_t params_length,
    uint32_t timeout)
{
    // Build frame data with command and parameters.
    uint8_t buff[PN532_FRAME_MAX_LENGTH];
    buff[0] = PN532_HOSTTOPN532;
    buff[1] = command & 0xFF;
    for (uint8_t i = 0; i < params_length; i++)
    {
        buff[2 + i] = params[i];
    }
    // Send frame and wait for response.
    if (PN532_WriteFrame('pn532', buff, params_length + 2) != PN532_STATUS_OK)
    {
        PN532_SPI_Wakeup();
        // pn532->log("Trying to wakeup");
        return PN532_STATUS_ERROR;
    }
    if (!PN532_SPI_WaitReady(timeout))
    {
        return PN532_STATUS_ERROR;
    }
    // Verify ACK response and wait to be ready for function response.
    PN532_SPI_ReadData(buff, sizeof(PN532_ACK));
    for (uint8_t i = 0; i < sizeof(PN532_ACK); i++)
    {
        if (PN532_ACK[i] != buff[i])
        {
            // pn532->log("Did not receive expected ACK from PN532!");
            return PN532_STATUS_ERROR;
        }
    }
    if (!PN532_SPI_WaitReady(timeout))
    {
        return PN532_STATUS_ERROR;
    }
    // Read response bytes.
    int frame_len = PN532_ReadFrame('pn532', buff, response_length + 2);

    // Check that response is for the called function.
    if (!((buff[0] == PN532_PN532TOHOST) && (buff[1] == (command + 1))))
    {
        // pn532->log("Received unexpected command response!");
        return PN532_STATUS_ERROR;
    }
    // Return response data.
    for (uint8_t i = 0; i < response_length; i++)
    {
        response[i] = buff[i + 2];
    }
    // The the number of bytes read
    return frame_len - 2;
}

/////
/**
 * @brief: Wait for a MiFare card to be available and return its UID when found.
 *     Will wait up to timeout seconds and return None if no card is found,
 *     otherwise a bytearray with the UID of the found card is returned.
 * @retval: Length of UID, or -1 if error.
 */
int PN532_ReadPassiveTarget(
    uint8_t *response,
    uint8_t card_baud,
    uint32_t timeout)
{
    // Send passive read command for 1 card.  Expect at most a 7 byte UUID.
    uint8_t params[] = {0x01, card_baud};
    uint8_t buff[19];
    int length = PN532_CallFunction('pn532', PN532_COMMAND_INLISTPASSIVETARGET,
                                    buff, sizeof(buff), params, sizeof(params), timeout);
    if (length < 0)
    {
        return PN532_STATUS_ERROR; // No card found
    }
    // Check only 1 card with up to a 7 byte UID is present.
    if (buff[0] != 0x01)
    {
        // pn532->log("More than one card detected!");
        return PN532_STATUS_ERROR;
    }
    if (buff[5] > 7)
    {
        // pn532->log("Found card with unexpectedly long UID!");
        return PN532_STATUS_ERROR;
    }
    for (uint8_t i = 0; i < buff[5]; i++)
    {
        response[i] = buff[6 + i];
    }
    return buff[5];
}

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

/*
    // init the pn532 functions
    pn532->reset = PN532_Reset;
    pn532->read_data = PN532_SPI_ReadData;
    pn532->write_data = PN532_SPI_WriteData;
    pn532->wait_ready = PN532_SPI_WaitReady;
    pn532->wakeup = PN532_SPI_Wakeup;
    pn532->log = PN532_Log;

    // hardware wakeup
    pn532->wakeup();
*/
/**************************************************************************
 * End: SPI
 **************************************************************************/