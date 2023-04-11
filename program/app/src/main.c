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

#define TEPLOMER1 0x49
#define TEPLOMER2 0x4A
#define TEPLOMER3 0x4B

////////////////////////////////////////////////////////////////////
//! Uživatelské funkce
void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    delay_init(); // Incializace časovače TIM4
    init_milis(); // Iniciaizace millis TIM2
    // uart_init();                                                 // Iniciaizace UART
    GPIO_Init(LED_PORT, LED_PIN_RED, GPIO_MODE_OUT_PP_LOW_SLOW); // Pin LED RED
    LCD_I2C_Init(0x27, 16, 2);                                   // Inicializace LCD
    LCD_I2C_Print("Inicializace...");                            // Úvodní obrazovka na displej
    LM75A_Init(TEPLOMER1, TEPLOMER2, TEPLOMER3);                 // Inicializace teploměrů
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(LED_PORT, LED_PIN_GREEN, GPIO_MODE_OUT_PP_LOW_SLOW); // PIN Inicializace proběhla úspěšně
    GPIO_WriteHigh(LED_PORT, LED_PIN_RED);                         // Konec inicializace
    delay_ms(1500);
    GPIO_WriteHigh(LED_PORT, LED_PIN_GREEN);
    GPIO_WriteLow(LED_PORT, LED_PIN_RED);
    LCD_I2C_Clear();
}
////////////////////////////////////////////////////////////////////
//! Main program loop
int main(void)
{
    //? Lokální proměnné
    uint8_t temperature_data[2]; // Proměnná pro uložení teploty
    // char prostredi[3] = {"pokoj1", "pokoj2", "venku"}; // Proměnná pro prostředí
    uint16_t mtime_key = 0;                                // Proměnná pro millis
    uint8_t adresy[3] = {TEPLOMER1, TEPLOMER2, TEPLOMER3}; // Proměnná pro skladování adres
    uint8_t cislo = 0;

    setup();                    // Inicializace všech periferií
    LCD_I2C_SetCursor(0, 0);    // Nastavení kurzoru
    LCD_I2C_Print("Teplota :"); // Úvodní obrazovka na displej
    while (1)
    {
        char buffer[48];

        if ((get_milis() - mtime_key) > 3000) // každých 1500 ms
        {
            mtime_key = get_milis();                                  // Milis now
            LM75A_ReadTemperature((adresy[cislo]), temperature_data); // Čtení teploty
            // uart_send(temperature_data, sizeof(temperature_data));
            GPIO_WriteReverse(GPIOD, GPIO_PIN_4);
            delay_ms(20);
            GPIO_WriteReverse(GPIOD, GPIO_PIN_4);
            LCD_I2C_SetCursor(0, 1);                                                               // Nastavení kurzoru
            sprintf(buffer, "T%d = %d.%d C", cislo + 1, temperature_data[0], temperature_data[1]); // Zformátování stringu // todo  >> 5
            LCD_I2C_Print(buffer);
            cislo++;
            if (cislo >= 3)
            {
                cislo = 0;
            }
        }
    }
}
////////////////////////////////////////////////////////////////////