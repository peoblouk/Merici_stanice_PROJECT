#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "milis.h"
#include "LCD_I2C_UI.h"
#include "rfid_rc522.h"

//! Makra
// Indikační LED
#define LED_PORT GPIOA
#define LED_PIN_GREEN GPIO_PIN_2
#define LED_PIN_RED GPIO_PIN_3

// Enkodér
#define ENCODER_PORT GPIOD
#define ENCODER_PIN_A GPIO_PIN_4
#define ENCODER_PIN_B GPIO_PIN_5

//! Proměnné
bool Status = FALSE;                                                // Proměnná pro kontrolu stavu systému
uint8_t CardID[5];                                                  // Proměnná pro uložení přečteného UID z karty
uint16_t *encoder_position = 0;                                     // Proměnná pro sledování pozice enkodéru
BitStatus aktualni_stav;                                            // Proměnná pro enkodér
BitStatus predchozi_stav;                                           // Proměnná pro enkodér
uint16_t mtime_key = 0;                                             // Proměnná pro millis
char *user_list[5] = {"Petr", "Franta", "Honza", "Karel", "David"}; // Uživatelé
char buffer[48];

//! Uživatelské funkce
void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    delay_init();                                                           // Incializace časovače TIM4
    init_milis();                                                           // Iniciaizace millis TIM2
    GPIO_Init(LED_PORT, LED_PIN_RED, GPIO_MODE_OUT_PP_LOW_SLOW);            // Pin LED RED
    LCD_I2C_Init(0x27, 16, 2);                                              // Inicializace LCD
    LCD_I2C_Print("Inicializace...");                                       // Úvodní obrazovka na displej
    TM_MFRC522_Init();                                                      // Incializace RFID readeru
    GPIO_Init(ENCODER_PORT, ENCODER_PIN_A, GPIO_MODE_IN_PU_IT);             // Inicializace enkodéru
    GPIO_Init(ENCODER_PORT, ENCODER_PIN_B, GPIO_MODE_IN_PU_IT);             // Incicializace enkodéru
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL); // Reakce portu D na náběžnou a sestupnou hranu
    ITC_SetSoftwarePriority(ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_1);            // Nastavení priority přerušení
    enableInterrupts();                                                     // Povolení přerušení globálně
    predchozi_stav = GPIO_ReadInputPin(ENCODER_PORT, ENCODER_PIN_A);        // Nastavení proměnné
    GPIO_Init(LED_PORT, LED_PIN_GREEN, GPIO_MODE_OUT_PP_LOW_SLOW);          // PIN Inicializace proběhla úspěšně
    GPIO_WriteHigh(LED_PORT, LED_PIN_RED);                                  // Konec inicializace
    delay_ms(1500);
    GPIO_WriteHigh(LED_PORT, LED_PIN_GREEN);
    GPIO_WriteLow(LED_PORT, LED_PIN_RED);
    LCD_I2C_Clear();
}

void otoceni_vpravo() // Otočení enkodéru vpravo
{

    encoder_position++;
}

void otoceni_nalevo() // Otočení enkodéru vlevo
{
    encoder_position--;
}

//! Main program loop
int main(void)
{
    setup(); // Inicializace všech periferií

    //? Uživatel přihlášen
    while (Status == TRUE)
    {
        if ((get_milis() - mtime_key) > 500) // každých 500 ms
        {
            LCD_I2C_SetCursor(0, 0);                   // Nastavení kurzoru
            LCD_I2C_Print("Prihlasen");                // Úvodní obrazovka na displej
            LCD_I2C_SetCursor(0, 1);                   // Nastavení kurzoru
            sprintf(buffer, "user: %s", user_list[0]); // Formátování stringu
            LCD_I2C_Print(buffer);                     // Úvodní obrazovka na displej
        }
    }

    //? Uživatel nepřihlášen
    while (Status == FALSE)
    {
        if ((get_milis() - mtime_key) > 500) // každých 500 ms
        {
            mtime_key = get_milis();               // milis now
            LCD_I2C_SetCursor(0, 0);               // Nastavení kurzoru
            LCD_I2C_Print("Priloz kartu");         // Úvodní obrazovka na displej
            if (TM_MFRC522_Check(CardID) == MI_OK) // Karta nalezena
            {
                Status = TRUE;
                sprintf(buffer, "%x", CardID);
                LCD_I2C_Print(buffer); // Print ID karty na displej
            }
            if (TM_MFRC522_Check(CardID) == MI_ERR) // Karta nenalezena
            {
                Status = FALSE;
                LCD_I2C_SetCursor(0, 1);
                sprintf(buffer, "UID: %x", CardID);
                LCD_I2C_Print(buffer));
            }

            // Kontrola encoderu
            LCD_I2C_SetCursor(14, 1);
            sprintf(buffer, "%d", encoder_position);
            LCD_I2C_Print(buffer); // TODO zjistit, zda se nebude být s ostatními sprintf
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
    }
    delay_ms(4);
    predchozi_stav = aktualni_stav;
}
