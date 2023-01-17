#include "stm8s.h"         // Libraries SPL
#include "delay.h"         // Library for delay
#include "serialmonitor.h" // Library for serial monitoring
#include "LCD_I2C.h"       // Library for LCD1602

#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_4

void setup()
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    delay.init();                                  // Inicializace delay (TIM4)
    Serial.begin(9600);                            // Inicializace Serial monitoru
    // LCD_I2C_Init(adresa, 16, 2);                   // Inicializace LCD (adresa, )
    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
}

int main(void)
{
    setup(); // Funkce setup
    while (1)
    {
        GPIO_WriteReverse(LED_PORT, LED_PIN);
        delay.ms(500); // Delay na 0,5 s
        Serial.print_str("test\n");
        // LCD_I2C_SetCursor(1,1); // LCD set cursor
    }
}