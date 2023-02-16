/* ------------------------------- Import knihoven ------------------------------------------ */
#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "Serial.h"
/* ------------------------------------------------------------------------------------------- */
/* ------------------------------ Uživatelské makra ------------------------------------------ */

// Indikační LED
#define LED_PORT_GREEN GPIOA
#define LED_PIN_GREEN GPIO_PIN_1
#define LED_PORT_RED GPIOD
#define LED_PIN_RED GPIO_PIN_2
/* ------------------------------ Uživatelské funkce ----------------------------------------- */
void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz7
    // delay.init();                                                        // Inicializqace delay
    GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_SLOW); // Control LED
    // GPIO_Init(LED_PORT_GREEN, LED_PIN_GREEN, GPIO_MODE_OUT_PP_LOW_SLOW); // Inicializace LED green
    // GPIO_Init(LED_PORT_RED, LED_PIN_RED, GPIO_MODE_OUT_PP_LOW_SLOW);     // Inicializace LED red

    Serial_begin(9600); // Incializace Serial monitoru
    Serial_print("Serial begin\n");
    LCD_I2C_Init(0x27, 16, 2); // Inicializace LCD
    LCD_I2C_SetCursor(0, 0);
    LCD_I2C_Print("Security system\n");
    // LCD_I2C_SetCursor(1, 0);
    // LCD_I2C_Print("Petr Oblouk\n");
    // Serial_print("Init succesfull\n");
}
/* ------------------------------------------------------------------------------------------- */
void loop(void)
{
    // GPIO_WriteReverse(GPIOE, GPIO_PIN_5); // Test if flash and init was successful
    // Serial_print("test__L\n");
}
/* ------------------------------ Hlavní program --------------------------------------------- */
int main(void)
{
    setup(); // Inicializace periferíí a úvodní text na LCD obrazovku
    while (1)
    {
        loop();
    }
}
/* ------------------------------------------------------------------------------------------- */