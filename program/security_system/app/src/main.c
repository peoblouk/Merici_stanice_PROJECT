#include "stm8s.h"         // Libraries SPL
#include "delay.h"         // Library for delay
#include "serialmonitor.h" // Library for serial monitoring
#include "LCD_I2C.h"       // Library for LCD1602
#include "loc_rc522.h"

#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_4

void setup()
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    delay.init();                                  // Inicializace delay (TIM4)
    Serial.begin(9600);                            // Inicializace Serial monitoru
    // LCD_I2C_Init(0x26, 16, 2);                   // Inicializace LCD (adresa, sloupce, řádky )
    RFID.init(); // Inicializace RFID

    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
}

int main(void)
{
    setup(); // Funkce setup
    while (1)
    {
        GPIO_WriteReverse(LED_PORT, LED_PIN);
        delay.ms(500); // Delay na 0,5 s

        // LCD_I2C_SetCursor(0, 0); // LCD set cursor
        // LCD_I2C_Print("Security system");
        Serial.print_str("test\n");
    }
}