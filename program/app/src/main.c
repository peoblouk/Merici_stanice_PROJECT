#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "milis.h"
#include "lm75a.h"

//! Makra
// Indikační LED
#define LED_PORT GPIOA
#define LED_PIN_GREEN GPIO_PIN_2
#define LED_PIN_RED GPIO_PIN_3

#define TEPLOMER1 0x49
#define TEPLOMER2 0x4A
#define TEPLOMER3 0x4B

//! Proměnné
uint16_t mtime_key = 0; // Proměnná pro millis

//! Uživatelské funkce
void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    delay_init();                                                // Incializace časovače TIM4
    init_milis();                                                // Iniciaizace millis TIM2
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

//! Main program loop
int main(void)
{
    float teplomer1, teplomer2, teplomer3;
    char zobraz[3] = {"t1", "t2", "t3"};

    setup();                   // Inicializace všech periferií
    LCD_I2C_SetCursor(0, 0);   // Nastavení kurzoru
    LCD_I2C_Print("Teplota:"); // Úvodní obrazovka na displej
    while (1)
    {

        char buffer[48];

        teplomer2 = LM75A_Temperature(TEPLOMER2);
        teplomer3 = LM75A_Temperature(TEPLOMER3);

        // if ((get_milis() - mtime_key) > 1500) // každých 1500 ms
        // {
        // mtime_key = get_milis(); // milis now
        GPIO_WriteReverse(GPIOD, GPIO_PIN_4);

        LCD_I2C_SetCursor(0, 1); // Nastavení kurzoru
        teplomer1 = LM75A_Temperature(TEPLOMER1);
        sprintf(buffer, "T1= %.1f C", teplomer1);
        LCD_I2C_Print(buffer);

        // for (int i = 0; i < 3; i++)
        // {
        //     LCD_I2C_SetCursor(0, 1); // Nastavení kurzoru
        //     sprintf(buffer, "%c= %d", zobraz[i], teploty[i]);
        //     LCD_I2C_Print(buffer);
        //     delay_ms(100);
        // }
        delay_ms(1500);
    }
}
