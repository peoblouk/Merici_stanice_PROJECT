/* ------------------------------- Import knihoven ------------------------------------------*/
#include "stm8s.h"         // Libraries SPL
#include "delay.h"         // Library for delay
#include "serialmonitor.h" // Library for serial monitoring
#include "LCD_I2C.h"       // Library for LCD1602
// #include "rfid_rc522.h"    // Library for RFID RC522
// #include "milis.h"         // Library for milis
/* -------------------------------------------------------------------------------------------*/
/* ------------------------------ Uživatelské makra ------------------------------------------*/
// Proměnné
uint16_t mtime_key = 0; // Pro millis
bool status;            // Pro kontrolu statusu
uint8_t CardID[5];

// Indikační LED
#define LED_PORT_GREEN GPIOA
#define LED_PIN_GREEN GPIO_PIN_1
#define LED_PORT_RED GPIOD
#define LED_PIN_RED GPIO_PIN_2

// Enkodér
#define ENCODER_PORT GPIOD
#define ENCODER_PIN_A GPIO_PIN_2
#define ENCODER_PIN_B GPIO_PIN_3
#define pinA GPIO_ReadInputPin(ENCODER_PORT, ENCODER_PIN_A)
#define pinB GPIO_ReadInputPin(GPIOC, ENCODER_PIN_B)
uint8_t encoder_position = 0; // Encoder pozice

/* ------------------------------ Uživatelské funkce -----------------------------------------*/
void setup()
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    delay_init();                                  // Inicializace delay (TIM4)
    Serial_begin(9600);                            // Inicializace Serial monitoru
    // TM_MFRC522_Init();
    // init_milis();              // Inicializace millis
    LCD_I2C_Init(0x27, 16, 2); // Inicializace LCD (adresa, sloupce, řádky )
    // GPIO_Init(ENCODER_PORT, ENCODER_PIN_A | ENCODER_PIN_B, GPIO_MODE_IN_FL_NO_IT); // Inicilaizace Encoderu
    GPIO_Init(LED_PORT_GREEN, LED_PIN_GREEN, GPIO_MODE_OUT_PP_LOW_SLOW); // Inicializace LED green
    GPIO_Init(LED_PORT_RED, LED_PIN_RED, GPIO_MODE_OUT_PP_LOW_SLOW);     // Inicializace LED red
    status = FALSE;                                                      // Reset statusu
    Serial_print_string("init successful\n");
}

/* -------------------------------------------------------------------------------------------*/
/* ------------------------------ Hlavní program ---------------------------------------------*/
int main(void)
{
    setup(); // Inicializace periferii
    while (1)
    {
        LCD_I2C_SetCursor(0, 0);
        LCD_I2C_Print("Security system");

        /* if (get_milis() - mtime_key > 55) // Nezatěžuj procesor a zkontroluj každých 55 ms
        {

            mtime_key = get_milis(); // Aktualizuj milis
            // GPIO_WriteReverse(LED_PORT, LED_PIN); // Test upload
            if (status == FALSE)
            {
                GPIO_WriteReverse(LED_PORT_GREEN, LED_PIN_GREEN); // Test upload
               /*  if (TM_MFRC522_Check(CardID) == MI_OK)
                {
                    // CardID is valid
                    Serial_print_string("CardID is valid\n");
                    // Serial_print_string("%x, %x, %x, %x, %x \n", CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]); // Výpis karty
                }
                else
                {
                    // CardID is invalid
                    Serial_print_string("CardID not detected\n");
                } */
    }
}

/*
// Obsluha enkoderu
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6) // Obsluha přerušení při otočení encoderu
{
    // Nastavení otáčení encoderu

    if (pinA == 0 && pinB == 1)
    {
        encoder_position++;
    }
    else if (pinA == 1 && pinB == 0)
    {
        encoder_position--;
    }
}
*/

/* -------------------------------------------------------------------------------------------*/
