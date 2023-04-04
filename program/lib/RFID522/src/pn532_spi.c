#include "pn532_spi.h"

void pn532_init(void)
{
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE); // Nahrada za MFRC522_CS_RCC

    // Enable external pullup
    GPIO_ExternalPullUpConfig(SPI_PORT, (GPIO_Pin_TypeDef)(SPI_MISO | SPI_MOSI | SPI_SCK), ENABLE);

    // CS pin
    GPIO_Init(CHIP_SELECT_PORT, CHIP_SELECT_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW); // Inicializace CS
    GPIO_Init(SPI_PORT, SPI_RST, GPIO_MODE_OUT_PP_HIGH_SLOW);                 // Inicializace RST

    // Reset RFID čtečky
    GPIO_WriteHigh(SPI_PORT, SPI_RST);
    GPIO_WriteLow(SPI_PORT, SPI_RST);
    CS_H; // Konec komunikace

    SPI_DeInit(); // Reset SPI to default settings
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_4, SPI_MODE_MASTER,
             SPI_CLOCKPOLARITY_HIGH, SPI_CLOCKPHASE_2EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX,
             SPI_NSS_SOFT, 0x07); // Initialize SPI with desired settings
    SPI_Cmd(ENABLE);              // Enable SPI peripheral

    // Configure PN532 chip
    uint8_t tx_buf[6] = {0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4};
    uint8_t rx_buf[6] = {0};
    SPI_SendData(tx_buf[0]);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
        ;
    for (uint8_t i = 1; i < 6; i++)
    {
        rx_buf[i] = tx_buf[i];
        SPI_SendData(tx_buf[i]);
        while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET)
            ;
    }
}

uint8_t pn532_send_frame(uint8_t *frame, uint8_t len)
{
    CS_L;
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    tx_buf[len + 1]; // ulož do seznamu přijimaté bity
    rx_buf[len + 1];
    tx_buf[0] = PN532_PREAMBLE;
    tx_buf[1] = PN532_STARTCODE1;
    tx_buf[2] = PN532_STARTCODE2;
    tx_buf[3] = len + 1;
    tx_buf[4] = ~len + 1;

    for (uint8_t d = 0; d < len; d++)
    {
        tx_buf[5 + d] = frame[d];
    }
    uint8_t checksum = 0;
    for (uint8_t t = 0; t < len + 1; t++)
    {
        checksum += tx_buf[4 + t];
    }
    tx_buf[len + 5] = checksum;
    tx_buf[len + 6] = PN532_POSTAMBLE;
    SPI_SendData(tx_buf[0]);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
        ;
    for (uint8_t z = 1; z < len + 7; z++)
    {
        rx_buf[z] = tx_buf[z]; // todo výřešít
        SPI_SendData(tx_buf[z]);
        while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET) // Wait until response
            ;
    }
    CS_H; // Konec komunikace
    return rx_buf[7];
}

uint8_t pn532_read_data(uint8_t *data, uint8_t len2)
{
    uint8_t tx_buf[7] = {0x00, 0x00, 0xFF, 0x02, 0xFE, 0xD5, 0x03};
    uint8_t rx_buf[7] = {0};
    CS_L;
    SPI_SendData(tx_buf[0]);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
        ;
    for (uint8_t u = 1; u < 7; u++)
    {
        rx_buf[u] = tx_buf[u];
        SPI_SendData(tx_buf[u]);
        while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET)
            ;
    }
    // Wait for data to be available
    uint8_t timeout = 100;
    while ((SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET) && (--timeout))
    {
        delay_us(5);
    }
    if (!timeout)
    {
        return 0;
    }
    // Read data from buffer
    data[0] = SPI_ReceiveData();
    uint8_t len_received = 0;
    while ((len_received < len2) && (--timeout))
    {
        delay_us(5);
        if (SPI_GetFlagStatus(SPI_FLAG_RXNE) != RESET)
        {
            data[len_received++] = SPI_ReceiveData();
        }
    }
    CS_H;
    return len_received;
}