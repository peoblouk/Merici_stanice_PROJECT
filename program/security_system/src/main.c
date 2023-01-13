#include "stm8s.h"
#include "milis.h"

#include "stm8s_gpio.h"
// peripheries
#include "lcd1602_i2c.h" // Knihovna pro displej s I2C převodníkem
#include "keypad.h"      // Knihovna pro klávesnici
#include "rc552.h"

void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    init.milis();                                  // Incializace millis
    LCD.init();                                    // Incializace lcd1602
    key
}

int main(void)
{
    setup(); // Incializace periferii

    while (1) // Hlavní smyčka programu
    {
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
