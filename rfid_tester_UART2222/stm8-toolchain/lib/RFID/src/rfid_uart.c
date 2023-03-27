#include "rfid_uart.h"

#define RC522_CS_PORT GPIOC
#define RC522_CS_PIN GPIO_PIN_4

#define RC522_RST_PORT GPIOC
#define RC522_RST_PIN GPIO_PIN_3

void rc522_init(void)
{
    // Set up GPIO pins for CS and RST
    GPIO_Init(RC522_CS_PORT, RC522_CS_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(RC522_RST_PORT, RC522_RST_PIN, GPIO_MODE_OUT_PP_LOW_FAST);

    // Reset RC522
    GPIO_WriteHigh(RC522_RST_PORT, RC522_RST_PIN);
    GPIO_WriteLow(RC522_RST_PORT, RC522_RST_PIN);
    GPIO_WriteHigh(RC522_RST_PORT, RC522_RST_PIN);

    // Initialize SPI
    SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_16, SPI_MODE_MASTER,
             SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX,
             SPI_NSS_SOFT, 0x00);
    SPI_Cmd(ENABLE);
    SPI_SendData(PCD_IDLE);
    SPI_SendData(PCD_IDLE);
}

uint8_t rc522_read_register(uint8_t addr)
{
    uint8_t data;
    GPIO_WriteLow(RC522_CS_PORT, RC522_CS_PIN);
    SPI_SendData(addr << 1 | 0x80);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
        ;
    while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET)
        ;
    data = SPI_ReceiveData();
    GPIO_WriteHigh(RC522_CS_PORT, RC522_CS_PIN);
    return data;
}

void rc522_write_register(uint8_t addr, uint8_t val)
{
    GPIO_WriteLow(RC522_CS_PORT, RC522_CS_PIN);
    SPI_SendData(addr << 1 & 0x7E);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
        ;
    SPI_SendData(val);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
        ;
    GPIO_WriteHigh(RC522_CS_PORT, RC522_CS_PIN);
}

void rc522_antenna_on(void)
{
    uint8_t value = rc522_read_register(0x14);
    value |= 0x03;
    rc522_write_register(0x14, value);
}

void rc522_antenna_off(void)
{
    uint8_t value = rc522_read_register(0x14);
    value &= ~0x03;
    rc522_write_register(0x14, value);
}

uint8_t rc522_request(uint8_t req_mode, uint8_t *tag_type)
{
    uint8_t status;
    uint16_t back_bits;
    rc522_write_register(0x0D, 0x07);
    status = rc522_send_command(0x0C, &req_mode, 1, tag_type, &back_bits);
    if ((status != MI_OK) || (back_bits != 0x10))
    {
        status = MI_ERR;
    }
    return status;
}

uint8_t rc522_send_command(uint8_t cmd, uint8_t *data, uint8_t data_len, uint8_t *response, uint16_t *response_len)
{
    uint8_t status = MI_ERR;
    uint8_t irq_en = 0x00;
    uint8_t wait_irq = 0x00;
    uint8_t last_bits;
    uint8_t n;
    uint16_t i;

    switch (cmd)
    {
    case PCD_AUTHENT:
        irq_en = 0x12;
        wait_irq = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irq_en = 0x77;
        wait_irq = 0x30;
        break;
    default:
        break;
    }
    rc522_write_register(0x02, irq_en | 0x80);
    rc522_clear_register_bitmask(0x01, 0x80);
    rc522_set_register_bitmask(0x0A, 0x80);
    rc522_write_register(0x01, 0x00);
    for (i = 0; i < data_len; i++)
    {
        rc522_write_register(0x09, data[i]);
    }
    rc522_write_register(0x01, 0x00 | wait_irq);
    if (cmd == PCD_TRANSCEIVE)
    {
        rc522_set_register_bitmask(0x0D, 0x80);
    }
    i = 2000;
    while (1)
    {
        n = rc522_read_register(0x04);
        i--;
        if ((i == 0) || ((n & 0x01) && !(n & wait_irq)))
        {
            break;
        }
    }
    rc522_clear_register_bitmask(0x0A, 0x80);
    if (i != 0)
    {
        if ((rc522_read_register(0x06) & 0x1B) == 0x00)
        {
            status = MI_OK;
            if (n & irq_en & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (cmd == PCD_TRANSCEIVE)
            {
                n = rc522_read_register(0x0C);
                last_bits = rc522_read_register(0x0D) & 0x07;
                if (last_bits)
                {
                    *response_len = (n - 1) * 8 + last_bits;
                }
                else
                {
                    *response_len = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > 16)
                {
                    n = 16;
                }
                for (i = 0; i < n; i++)
                {
                    response[i] = rc522_read_register(0x09);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }
    return status;
}

void rc522_set_register_bitmask(uint8_t addr, uint8_t mask)
{
    uint8_t tmp;
    tmp = rc522_read_register(addr);
    rc522_write_register(addr, tmp | mask);
}

void rc522_clear_register_bitmask(uint8_t addr, uint8_t mask)
{
    uint8_t tmp;
    tmp = rc522_read_register(addr);
    rc522_write_register(addr, tmp & ~mask);
}

uint8_t rc522_card_select(uint8_t *uid)
{
    uint8_t i;
    uint8_t status;
    uint8_t size;
    uint16_t recv_bits;
    uint8_t buffer[9];
    buffer[0] = PICC_SELECTTAG;
    buffer[1] = 0x70;
    for (i = 0; i < 5; i++)
    {
        buffer[i + 2] = uid[i];
    }
    rc522_calculate_crc(buffer, 7, &buffer[7]);
    status = rc522_send_command(PCD_TRANSCEIVE, buffer, 9, buffer, &recv_bits);
    if ((status == MI_OK) && (recv_bits == 0x18))
    {
        size = buffer[0];
    }
    else
    {
        size = 0;
    }
    return size;
}

/* uint8_t rc522_request(uint8_t req_mode, uint8_t *tag_type)
{
    uint8_t status;
    uint16_t back_bits;
    rc522_clear_register_bitmask(0x0A, 0x80);
    rc522_write_register(0x0D, 0x80);
    uint8_t buffer[1];
    buffer[0] = req_mode;
    status = rc522_send_command(PCD_TRANSCEIVE, buffer, 1, buffer, &back_bits);
    if ((status != MI_OK) || (back_bits != 0x10))
    {
        return MI_ERR;
    }
    if (buffer[0] == 0x04)
    {
        *tag_type = TAG_TYPE_MIFARE_MINI;
    }
    else if (buffer[0] == 0x02)
    {
        *tag_type = TAG_TYPE_MIFARE_1K;
    }
    else if (buffer[0] == 0x01)
    {
        *tag_type = TAG_TYPE_MIFARE_UL;
    }
    else
    {
        *tag_type = TAG_TYPE_UNKNOWN;
    }
    return MI_OK;
} */

uint8_t rc522_anticoll(uint8_t *ser_num)
{
    uint8_t status;
    uint8_t i;
    uint8_t ser_num_check = 0;
    uint16_t recv_bits;
    uint8_t buffer[4];
    uint8_t buffer_size;
    buffer[0] = PICC_ANTICOLL;
    buffer[1] = 0x20;
    status = rc522_send_command(PCD_TRANSCEIVE, buffer, 2, buffer, &recv_bits);
    if (status == MI_OK)
    {
        buffer_size = recv_bits / 8;
        if (buffer_size == 5)
        {
            for (i = 0; i < 4; i++)
            {
                ser_num[i] = buffer[i];
                ser_num_check ^= buffer[i];
            }
            ser_num[4] = buffer[4];
            if (ser_num_check != buffer[4])
            {
                status = MI_ERR;
            }
        }
        else
        {
            status = MI_ERR;
        }
    }
    return status;
}

void rc522_calculate_crc(uint8_t *data, uint8_t length, uint8_t *result)
{
    uint8_t i;
    uint8_t n;
    rc522_clear_register_bitmask(0x0C, 0x80);
    rc522_set_register_bitmask(0x0A, 0x80);
    for (i = 0; i < length; i++)
    {
        rc522_write_register(0x09, *(data + i));
    }
    rc522_write_register(0x01, 0x03);
    i = 0xFF;
    while (1)
    {
        n = rc522_read_register(0x0C);
        i--;
        if (!(i != 0 && !(n & 0x04)))
        {
            break;
        }
    }
    result[0] = rc522_read_register(0x22);
    result[1] = rc522_read_register(0x21);
    rc522_clear_register_bitmask(0x0C, 0x80);
}