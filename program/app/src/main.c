/* ------------------------------- Import knihoven ------------------------------------------ */
#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "Serial.h"
/* ------------------------------------------------------------------------------------------- */
/* ------------------------------ Uživatelské makra ------------------------------------------ */

// Indikační LED systému
#define LED_PORT_GREEN GPIOA
#define LED_PIN_GREEN GPIO_PIN_1
#define LED_PORT_RED GPIOD
#define LED_PIN_RED GPIO_PIN_2
/* ------------------------------ Uživatelské funkce ----------------------------------------- */
void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz7
    delay.init();                                  // Inicializace delay
    Serial_begin(9600);                            // Incializace Serial monitoru
    Serial_print("Serial begin\n");                // Incializační hláška přes UART
    LCD_I2C_Init(0x27, 16, 2);                     // Inicializace LCD
    LCD_I2C_SetCursor(0, 0);                       // Nastavení kurzoru
    LCD_I2C_Print("Security system\n");            // Print na displej
}
/* ------------------------------------------------------------------------------------------- */
void loop(void)
{
}
/* ------------------------------ Hlavní program --------------------------------------------- */
int main(void)
{
    setup(); // Inicializace periferíí a úvodní text na LCD obrazovku
    /*     while (1)
        {
            loop();
        }
     */
}
/* ------------------------------------------------------------------------------------------- */