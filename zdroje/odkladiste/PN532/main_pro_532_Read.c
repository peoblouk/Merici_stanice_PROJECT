char *user_list[5] = {"Petr", "Franta", "Honza", "Karel", "David"}; // Uživatelé

void pn532_poslat(uint8_t *data, uint16_t length)
{
    if (length > PN532_FRAME_MAX_LENGTH || length < 1)
    {
        LCD_I2C_SetCursor(0, 0);
        LCD_I2C_Print("Too long frame");
        delay_ms(1000);
    }
    else
    {
        uint8_t frame[PN532_FRAME_MAX_LENGTH + 7];
        uint8_t checksum = 0;

        // Preamble
        frame[0] = PN532_PREAMBLE;
        // Start of Packet code
        frame[1] = PN532_STARTCODE1;
        frame[2] = PN532_STARTCODE2;
        // Packet of lenght
        for (uint8_t i = 0; i < 3; i++)
        {
            checksum += frame[i];
        }
        // Packet lenght checksum
        frame[3] = length & 0xFF;
        frame[4] = (~length + 1) & 0xFF;
        //! Packet data
        for (uint8_t i = 0; i < length; i++)
        {
            frame[5 + i] = data[i];
            checksum += data[i];
        }
        // Packet data checksum
        frame[length + 5] = ~checksum & 0xFF;
        // Postamble
        frame[length + 6] = PN532_POSTAMBLE;

        // LCD data check
        LCD_I2C_SetCursor(0, 0);
        LCD_I2C_Print("SPI frame size:");
        LCD_I2C_SetCursor(0, 1);
        sprintf(buffer, "%d", length);
        LCD_I2C_Print(buffer);
        uint16_t response[];
        //

        CS_L;                                     // začátek komunikace
        for (uint16_t z = 0; z < length + 7; z++) // Pošli frame
        {
            SPI_SendData(frame[z]);
            response[z] = SPI_ReceiveData();
            CS_H;
            while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
                ; // počkej dokud nebude trasmit buffer prázdný
        }
        CS_H;
    }
}

// block fo init try

uint8_t init[8] = {0x00, 0x00, 0xFF, 0x01, 0xFF, 0xD4, 0x02, 0x2A};
uint8_t txx_buf[6] = {0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4};
uint8_t rxx_buf[12] = {0};

uint8_t params[] = {PN532_HOSTTOPN532, PN532_COMMAND_GETFIRMWAREVERSION & 0xFF};
pn532_poslat(params, sizeof(params));
