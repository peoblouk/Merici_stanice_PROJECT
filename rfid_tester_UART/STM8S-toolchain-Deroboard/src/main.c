#include "stm8s.h"
#include "delay.h"
#include "rfid_uart.h"

#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_4

void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    delay_init();
    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    RFID_Begin();
}

int main(void)
{
    setup();

    while (1)
    {
        GPIO_WriteReverse(LED_PORT, LED_PIN);
        delay_ms(50);
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
