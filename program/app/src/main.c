#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "milis.h"
#include "lm75a.h"
#include "uart_bridge.h"
// #include <string.h>
////////////////////////////////////////////////////////////////////
//! Makra
// Indikační LED
#define LED_PORT GPIOA
#define LED_PIN_GREEN GPIO_PIN_2
#define LED_PIN_RED GPIO_PIN_3
#define BUILD_IN_LED GPIOD, GPIO_PIN_4

#define TEPLOMER1 0x49
#define TEPLOMER2 0x4A
#define TEPLOMER3 0x4B

////////////////////////////////////////////////////////////////////
//! Uživatelské funkce
void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);                 // Předdělička DIV1
    delay_init();                                                  // Incializace časovače TIM4
    init_milis();                                                  // Iniciaizace millis TIM2
    GPIO_Init(LED_PORT, LED_PIN_RED, GPIO_MODE_OUT_PP_LOW_SLOW);   // Pin LED RED
    LCD_I2C_Init(0x27, 16, 2);                                     // Inicializace LCD
    LCD_I2C_Print("Inicializace...");                              // Úvodní obrazovka na displej
    LM75A_Init(TEPLOMER1, TEPLOMER2, TEPLOMER3);                   // Inicializace teploměrů
    GPIO_Init(BUILD_IN_LED, GPIO_MODE_OUT_PP_LOW_SLOW);            // Inicializace LED
    GPIO_Init(LED_PORT, LED_PIN_GREEN, GPIO_MODE_OUT_PP_LOW_SLOW); // PIN Inicializace proběhla úspěšně
    GPIO_WriteHigh(LED_PORT, LED_PIN_RED);                         // Konec inicializace
    delay_ms(1500);                                                // Pockěj 1,5 s
    GPIO_WriteHigh(LED_PORT, LED_PIN_GREEN);                       // Zhasni LED
    LCD_I2C_Clear();                                               // Vyčisti displej
}
////////////////////////////////////////////////////////////////////
//! Main program loop
int main(void)
{
    //? Lokální proměnné
    uint8_t temperature_data[2];                           // Proměnná pro uložení teploty
    uint16_t mtime_key = 0;                                // Proměnná pro millis
    uint8_t adresy[3] = {TEPLOMER1, TEPLOMER2, TEPLOMER3}; // Proměnná pro skladování adres
    uint8_t cislo = 0;                                     // Proměnná pro scrolling čísla
    char buffer[48];                                       // Proměnná pro zápis na displej
    setup();                                               // Inicializace všech periferií
    LCD_I2C_SetCursor(0, 0);                               // Nastavení kurzoru
    LCD_I2C_Print("Teplota :");                            // Úvodní obrazovka na displej
    while (1)
    {
        if ((get_milis() - mtime_key) > 3000) // každých 1500 ms
        {
            mtime_key = get_milis();                                                               // Milis now
            LM75A_ReadTemperature((adresy[cislo]), temperature_data);                              // Čtení teploty
            GPIO_WriteReverse(BUILD_IN_LED);                                                       // Rozsviť BUILD_IN_LED
            delay_ms(20);                                                                          // Počkej 20 ms
            GPIO_WriteReverse(BUILD_IN_LED);                                                       // Zhasni BUILD_IN_LED
            LCD_I2C_SetCursor(0, 1);                                                               // Nastavení kurzoru
            sprintf(buffer, "T%d = %d.%d C", cislo + 1, temperature_data[0], temperature_data[1]); // Zformátování stringu
            LCD_I2C_Print(buffer);                                                                 // Vytiskni na displej
            cislo++;                                                                               // Pčičti k proměnné
            if (cislo >= 3)                                                                        // Přetečení teploměrů
            {
                cislo = 0;
                GPIO_WriteLow(LED_PORT, LED_PIN_RED);
                delay_ms(50);
                GPIO_WriteHigh(LED_PORT, LED_PIN_RED);
            }
            if (temperature_data[0] >= 20) // Pokud je teplota větší jak 20 stupňů tak rozstviť LED
            {
                GPIO_WriteLow(LED_PORT, LED_PIN_GREEN);
            }
            else
            {
                GPIO_WriteHigh(LED_PORT, LED_PIN_GREEN);
            }
        }
    }
}
////////////////////////////////////////////////////////////////////