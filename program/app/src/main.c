#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "rfid_rc522.h"
#include "milis.h"

//! Makra
// Indikační LED
#define LED_PORT GPIOA
#define LED_PIN_GREEN GPIO_PIN_2
#define LED_PIN_RED GPIO_PIN_3

// Enkodér
int encoder_position = 0; // Proměnná pro sledování pozice enkodéru
#define ENCODER_PORT GPIOD
#define ENCODER_PIN_A GPIO_PIN_4
#define ENCODER_PIN_B GPIO_PIN_5

//! Proměnné
bool Status = FALSE;      // Proměnná pro kontrolu stavu systému
uint8_t CardID[5];        // Proměnná pro uložení přečteného UID z karty
BitStatus aktualni_stav;  // Proměnná pro enkodér
BitStatus predchozi_stav; // Proměnná pro enkodér
uint16_t mtime_key = 0;   // Proměnná pro millis

//! Uživatelské funkce
void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    delay_init();                                                // Incializace časovače TIM4
    init_milis();                                                // Iniciaizace millis TIM2
    GPIO_Init(LED_PORT, LED_PIN_RED, GPIO_MODE_OUT_PP_LOW_SLOW); // Pin LED RED

    // displej
    LCD_I2C_Init(0x27, 16, 2); // Inicializace LCD
    LCD_I2C_SetCursor(0, 0);   // Nastavení kurzoru
    LCD_I2C_Print("user:");    // Úvodní obrazovka na displej

    // čtečka
    TM_MFRC522_Init(); // Incializace RFID readeru

    // enkodér
    GPIO_Init(ENCODER_PORT, ENCODER_PIN_A, GPIO_MODE_IN_PU_IT);             // Inicializace enkodéru
    GPIO_Init(ENCODER_PORT, ENCODER_PIN_B, GPIO_MODE_IN_PU_IT);             // Incicializace enkodéru
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL); // Reakce portu D na náběžnou a sestupnou hranu
    ITC_SetSoftwarePriority(ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_1);            // Nastavení priority přerušení
    enableInterrupts();                                                     // Povolení přerušení globálně
    predchozi_stav = GPIO_ReadInputPin(ENCODER_PORT, ENCODER_PIN_A);

    GPIO_Init(LED_PORT, LED_PIN_GREEN, GPIO_MODE_OUT_PP_LOW_SLOW); // PIN Inicializace proběhla úspěšně
    GPIO_WriteHigh(LED_PORT, LED_PIN_RED);                         // Konec inicializace
}

void otoceni_vpravo()
{
    encoder_position++;
}

void otoceni_nalevo()
{
    encoder_position--;
}

int main(void)
{
    setup();
    while (1)
    {
        if ((get_milis() - mtime_key) > 100)
        {
            mtime_key = get_milis(); // milis now

            if (TM_MFRC522_Check(CardID) == MI_OK)
            {
                // Serial_Print_String("Čtení proběhlo správně\n");
                Status = TRUE;
                LCD_I2C_Print(CardID); // Print ID karty na displej
            }
            if (TM_MFRC522_Check(CardID) == MI_ERR)
            {
                // Serial_Print_String("Žádná karta nebyla naleza\n");
                Status = FALSE;
            }

            LCD_I2C_SetCursor(6, 0);
            char enkoder;
            sprintf(enkoder, "%d", encoder_position);
            LCD_I2C_Print(enkoder);
        }
    }
}

//! Obsluha přerušení
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 5)
{
    BitStatus aktualni_stav = GPIO_ReadInputPin(ENCODER_PORT, ENCODER_PIN_B); // Proměnná pro enkodér
    if (aktualni_stav != (predchozi_stav && !aktualni_stav))
    {
        if (GPIO_ReadInputPin(ENCODER_PORT, ENCODER_PIN_A) != aktualni_stav)
        {
            otoceni_vpravo();
        }
        else
        {
            otoceni_nalevo();
        }
        delay_ms_2(1);
    }
    predchozi_stav = aktualni_stav;
}
