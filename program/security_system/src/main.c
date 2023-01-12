#include "stm8s.h"
#include "milis.h"

#include "stm8s_gpio.h"


void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    init.milis();                                  // Incializace millis
}

int main(void)
{
    uint32_t time = 0;

    setup(); // Incializace periferii

    while (1) // Hlavní smyčka programu
    {
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
