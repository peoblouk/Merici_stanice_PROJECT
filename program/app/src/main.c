#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "milis.h"
#include <string.h>
#include "lm75a.h"

//! Makra
// Indikační LED
#define LED_PORT GPIOA
#define LED_PIN_GREEN GPIO_PIN_2
#define LED_PIN_RED GPIO_PIN_3

//! Proměnné
bool Status = FALSE; // Proměnná pro kontrolu stavu systému
// uint8_t CardID[5];                                                  // Proměnná pro uložení přečteného UID z karty
uint16_t *encoder_position = 0; // Proměnná pro sledování pozice enkodéru
uint16_t mtime_key = 0;         // Proměnná pro millis
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

    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(LED_PORT, LED_PIN_GREEN, GPIO_MODE_OUT_PP_LOW_SLOW); // PIN Inicializace proběhla úspěšně
    GPIO_WriteHigh(LED_PORT, LED_PIN_RED);                         // Konec inicializace
    delay_ms(1500);
    GPIO_WriteHigh(LED_PORT, LED_PIN_GREEN);
    GPIO_WriteLow(LED_PORT, LED_PIN_RED);
    LCD_I2C_Clear();
}

//! Main program loop
int main(void)
{
    setup(); // Inicializace všech periferií

    while (Status == TRUE)
    {
        if ((get_milis() - mtime_key) > 500) // každých 500 ms
        {
            LCD_I2C_SetCursor(0, 0);    // Nastavení kurzoru
            LCD_I2C_Print("Prihlasen"); // Úvodní obrazovka na displej
        }
    }

    while (Status == FALSE)
    {
        // LCD_I2C_Clear();
        if ((get_milis() - mtime_key) > 1500) // každých 1500 ms
        {
            mtime_key = get_milis();  // milis now
            LCD_I2C_SetCursor(0, 0);  // Nastavení kurzoru
            LCD_I2C_Print("Teplota"); // Úvodní obrazovka na displej

            uint8_t data_len = pn532_read_data(data, MAX_FRAME_LEN);
            LCD_I2C_Print(frame);
        }
    }
}