#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "milis.h"
#include "pn532_spi.h"
#include <string.h>

//! Makra
// Indikační LED
#define LED_PORT GPIOA
#define LED_PIN_GREEN GPIO_PIN_2
#define LED_PIN_RED GPIO_PIN_3

//! Proměnné
bool Status = FALSE; // Proměnná pro kontrolu stavu systému
// uint8_t CardID[5];                                                  // Proměnná pro uložení přečteného UID z karty
uint16_t *encoder_position = 0;                                     // Proměnná pro sledování pozice enkodéru
uint16_t mtime_key = 0;                                             // Proměnná pro millis
char *user_list[5] = {"Petr", "Franta", "Honza", "Karel", "David"}; // Uživatelé
char buffer[48];

//! Uživatelské funkce
void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    delay_init();                                                // Incializace časovače TIM4
    init_milis();                                                // Iniciaizace millis TIM2
    GPIO_Init(LED_PORT, LED_PIN_RED, GPIO_MODE_OUT_PP_LOW_SLOW); // Pin LED RED
    LCD_I2C_Init(0x27, 16, 2);                                   // Inicializace LCD
    LCD_I2C_Print("Inicializace...");                            // Úvodní obrazovka na displej
    // RC522_Init();                                                // Incializace RFID readeru
    pn532_init();
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(LED_PORT, LED_PIN_GREEN, GPIO_MODE_OUT_PP_LOW_SLOW); // PIN Inicializace proběhla úspěšně
    GPIO_WriteHigh(LED_PORT, LED_PIN_RED);                         // Konec inicializace
    delay_ms(1500);
    GPIO_WriteHigh(LED_PORT, LED_PIN_GREEN);
    GPIO_WriteLow(LED_PORT, LED_PIN_RED);
    LCD_I2C_Clear();
}

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

        LCD_I2C_SetCursor(0, 0);
        LCD_I2C_Print("SPI frame size:");
        LCD_I2C_SetCursor(0, 1);
        sprintf(buffer, "%d", length);
        LCD_I2C_Print(buffer);
        uint16_t response[];

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
//! Main program loop
int main(void)
{
    setup(); // Inicializace všech periferií

    //? Uživatel přihlášen
    while (Status == TRUE)
    {
        if ((get_milis() - mtime_key) > 500) // každých 500 ms
        {
            LCD_I2C_SetCursor(0, 0);                   // Nastavení kurzoru
            LCD_I2C_Print("Prihlasen");                // Úvodní obrazovka na displej
            LCD_I2C_SetCursor(0, 1);                   // Nastavení kurzoru
            sprintf(buffer, "user: %s", user_list[0]); // Formátování stringu
            LCD_I2C_Print(buffer);                     // Úvodní obrazovka na displej
        }
    }

    //? Uživatel nepřihlášen
    while (Status == FALSE)
    {
        uint8_t init[8] = {0x00, 0x00, 0xFF, 0x01, 0xFF, 0xD4, 0x02, 0x2A};
        uint8_t txx_buf[6] = {0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4};
        uint8_t rxx_buf[12] = {0};

        uint8_t params[] = {PN532_HOSTTOPN532, PN532_COMMAND_GETFIRMWAREVERSION & 0xFF};

        pn532_poslat(params, sizeof(params));

        delay_ms(2000);
        LCD_I2C_Clear();
        // if ((get_milis() - mtime_key) > 500) // každých 500 ms
        // {
        //     mtime_key = get_milis();       // milis now
        //     LCD_I2C_SetCursor(0, 0);       // Nastavení kurzoru
        //     LCD_I2C_Print("Priloz kartu"); // Úvodní obrazovka na displej
        //     ////
        //     // Create a data frame to send to the PN532 chip
        //     uint8_t frame[] = {0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4, 0x14, 0x01, 0x17};
        //     uint8_t frame_len = sizeof(frame) / sizeof(frame[0]);

        //     // Send the data frame to the PN532 chip
        //     pn532_send_frame(frame, frame_len);

        //     // Read the response data from the PN532 chip
        //     uint8_t data[MAX_FRAME_LEN];
        //     uint8_t data_len = pn532_read_data(data, MAX_FRAME_LEN);
        //     LCD_I2C_Print(frame);

        //     ////
        // }
        // else
        // {
        //     // printf("Found card with UID: ");
        //     // for (uint8_t i = 0; i < data_len; i++)
        //     {
        //         // printf("%02x ", uid[i]);
        //         LCD_I2C_SetCursor(2, 1);
        //         // sprintf(buffer, "%02x ", uid[i]);
        //         // LCD_I2C_Print(buffer);
        //     }
        // }
    }
}

/*         for (uint8_t i = 0; i < sizeof(init); i++)
        {
            SPI_SendData(init[i]);
            rxx_buf[i] = SPI_ReceiveData();
            while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
                ; // počkej dokud nebude trasmit buffer prázdný
            GPIO_WriteReverse(LED_PORT, LED_PIN_RED);
        }
        unsigned char response[12];
        for (uint8_t z = 0; z < 12; z++)
        {
            SPI_SendData(0xFF);
            response[z] = SPI_ReceiveData();
            while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET)
                ; // počkej dokud nebude receive buffer prázdný
        }
 */
