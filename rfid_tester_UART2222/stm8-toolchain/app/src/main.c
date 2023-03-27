#include "stm8s.h"
#include "delay.h"
#include "rfid_uart.h"

#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_4

uint8_t status;
uint8_t tag_type;
uint8_t ser_num[5];
uint8_t i;

void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    delay_init();
    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    rc522_init();
}

int main(void)
{
    setup();

    while (1)
    {
        // Check for a card and get its type and serial number
        status = rc522_request(PICC_REQIDL, &tag_type);
        if (status == MI_OK)
        {
            status = rc522_anticoll(ser_num);
            if (status == MI_OK)
            {
                // Card detected, turn on LED
                GPIO_WriteLow(LED_PORT, LED_PIN);

                // Print card serial number
                // for (i = 0; i < 5; i++)
                // {
                //     printf("%02X ", ser_num[i]);
                // }
                // printf("\n");
            }
        }
        else
        {
            // No card detected, turn off LED
            GPIO_WriteHigh(LED_PORT, LED_PIN);
        }
        delay_ms(200);
    }
}
