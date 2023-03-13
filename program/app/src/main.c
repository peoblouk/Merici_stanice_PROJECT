#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "rfid_rc522.h"
#include "Serial.h"

//! Makra
// Indikační LED
#define LED_PORT_GREEN GPIOA
#define LED_PIN_GREEN GPIO_PIN_1
#define LED_PORT_RED GPIOD
#define LED_PIN_RED GPIO_PIN_2

/* //! Proměnné */
bool Status = FALSE; // Proměnná pro kontrolu stavu systému
uint8_t CardID[5];   // Proměnná pro uložení přečteného UID z karty

void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    Serial_Begin(9600); // Inicializace Serial monitoru
    Serial_Print_String("test");
    delay_init();                     // Incializace časovače TIM4
    LCD_I2C_Init(0x27, 16, 2);        // Inicializace LCD
    LCD_I2C_SetCursor(0, 0);          // Nastavení kurzoru
    LCD_I2C_Print("Security system"); // Úvodní obrazovka na displej
    TM_MFRC522_Init();                // Incializace RFID readeru
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_SLOW);
    // enableInterrupts(); // Povolení přeruření globálně
}

int main(void)
{
    setup();

    while (1)
    {
        GPIO_WriteReverse(GPIOD, GPIO_PIN_4);
        if (TM_MFRC522_Check(CardID) == MI_OK)
        {
            // Serial_Print_String("Čtení proběhlo správně\n");
            Status = TRUE;
            LCD_I2C_SetCursor(0, 1);
            // sprintf("")
            LCD_I2C_Print(CardID); // Print ID karty na displej
        }
        if (TM_MFRC522_Check(CardID) == MI_ERR)
        {
            // Serial_Print_String("Žádná karta nebyla naleza\n");
            Status = FALSE;
            LCD_I2C_SetCursor(0, 1);
            LCD_I2C_Print("nic");
        }
    }
}