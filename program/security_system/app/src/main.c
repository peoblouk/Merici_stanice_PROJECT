/* ------------------------------- Import knihoven ------------------------------------------*/
#include "stm8s.h"         // Libraries SPL
#include "delay.h"         // Library for delay
#include "serialmonitor.h" // Library for serial monitoring
#include "LCD_I2C.h"       // Library for LCD1602
#include "RC522.h"         // Library for RFID RC522
#include "milis.h"         // Library for milis
/* -------------------------------------------------------------------------------------------*/
/* ------------------------------ Uživatelské makra ------------------------------------------*/
uint16_t mtime_key = 0;
unsigned char g_ucTempbuf[20];
unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_4
/* ------------------------------ Uživatelské funkce -----------------------------------------*/
void setup()
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    delay.init();                                  // Inicializace delay (TIM4)
    Serial.begin(9600);                            // Inicializace Serial monitoru
    SPIRC522_Init();                               // Inicializace RFID
    init_milis();                                  // Inicializace millis
    // LCD_I2C_Init(0x26, 16, 2);                  // Inicializace LCD (adresa, sloupce, řádky )

    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    Serial.print_str("init successful\n");
}

/* -------------------------------------------------------------------------------------------*/
/* ------------------------------ Hlavní program ---------------------------------------------*/
int main(void)
{
    setup(); // Inicializace periferii
    while (1)
    {
        if (get_milis() - mtime_key > 55) // Nezatěžuj procesor a zkontroluj každých 55 ms
        {
            mtime_key = get_milis(); // Aktualizuj milis
            GPIO_WriteReverse(LED_PORT, LED_PIN);
        }
    }
}
/* -------------------------------------------------------------------------------------------*/

/*        //////////////////////////
        status = PcdRequest(PICC_REQALL, g_ucTempbuf);
        ////////////////////////////////////////////////////////////////////////
        if (status != MI_OK)
        {
            GPIO_WriteHigh(LED_PORT, LED_PIN);
            continue;
        }
        else
        {
            GPIO_WriteLow(LED_PORT, LED_PIN);
        }
        ////////////////////////////////////////////////////////////
        status = PcdAnticoll(g_ucTempbuf);
        /////////////////////////////////////////////////////////////
        if (status != MI_OK)
        {
            continue;
        }
        //////////////////////////////////////////////////////////////
        status = PcdSelect(g_ucTempbuf);
        if (status != MI_OK)
        {
            continue;
        }
        ////////////////////////////////////////////////////////////////////
        status = PcdAuthState(PICC_AUTHENT1A, 1, DefaultKey, g_ucTempbuf);
        if (status != MI_OK)
        {
            continue;
        }
        // LCD_I2C_SetCursor(0, 0); // LCD set cursor
        // LCD_I2C_Print("Security system");

        ///////////////////////////////////////////////////////
        status = PcdRead(1, g_ucTempbuf);
        if (status != MI_OK)
        {
            // continue;
        }
        for (int i = 0; i < 3; i++)
        {
            Serial.print_str(g_ucTempbuf[i]);
        }
        PcdHalt();*/
