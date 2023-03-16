#include "stm8s.h"
#include "delay.h"
#include "LCD_I2C.h"
#include "rfid_rc522.h"
#include "Serial.h"
// #include "encoder.h"

//! Makra
// Indikační LED
#define LED_PORT_GREEN GPIOA
#define LED_PIN_GREEN GPIO_PIN_2
#define LED_PORT_RED GPIOA
#define LED_PIN_RED GPIO_PIN_3

// Enkodér
int encoder_position = 0; // Encoder pozice
Encoder_Config ENCODER_CONFIG = {
    .clk = GPIO_PIN_4,
    .ds = GPIO_PIN_5,
    .onLeft = &handleLeft,
    .onRight = &handleRight,
};

#define ENCODER_PORT GPIOD
#define ENCODER_PIN_A GPIO_PIN_4
#define ENCODER_PIN_B GPIO_PIN_5
#define pinA GPIO_ReadInputPin(ENCODER_PORT, ENCODER_PIN_A)
#define pinB GPIO_ReadInputPin(ENCODER_PORT, ENCODER_PIN_B)

/* //! Proměnné */
bool Status = FALSE; // Proměnná pro kontrolu stavu systému
uint8_t CardID[5];   // Proměnná pro uložení přečteného UID z karty

void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    delay_init();                                                         // Incializace časovače TIM4
    GPIO_Init(LED_PORT_RED, LED_PIN_RED, GPIO_MODE_OUT_PP_LOW_SLOW);      // Pin LED RED
    LCD_I2C_Init(0x27, 16, 2);                                            // Inicializace LCD
    LCD_I2C_SetCursor(0, 0);                                              // Nastavení kurzoru
    LCD_I2C_Print("Security system");                                     // Úvodní obrazovka na displej
    TM_MFRC522_Init();                                                    // Incializace RFID readeru
    GPIO_Init(LED_PORT_GREEN, LED_PIN_GREEN, GPIO_MODE_OUT_PP_HIGH_SLOW); // PIN LED GREEN
    Encoder_Init(&ENCODER_CONFIG);                                        // Inicializace enkodéru

    // GPIO_Init(ENCODER_PORT, ENCODER_PIN_A, GPIO_MODE_IN_PU_IT);
    // GPIO_Init(ENCODER_PORT, ENCODER_PIN_B, GPIO_MODE_IN_PU_IT);
    // EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL);
    // ITC_SetSoftwarePriority(ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_1);
    // enableInterrupts();
}

void handleLeft()
{
    encoder_position++;
}

void handleRight()
{
    encoder_position--;
}

int main(void)
{
    setup();

    while (1)
    {
        GPIO_WriteReverse(LED_PORT_GREEN, LED_PIN_GREEN);
        GPIO_WriteReverse(LED_PORT_RED, LED_PIN_RED);
        delay_ms(250);
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
            LCD_I2C_SetCursor(0, 1);
        }
        // if (encoder_position >= 500)
        // {
        //     encoder_position = 0;
        // }
        // LCD_I2C_SetCursor(0, 1);
        char enkoder;
        sprintf(enkoder, "%d", encoder_position);
        LCD_I2C_Print(enkoder);
    }
}

/* INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 5)
{
    if (pinA == RESET && pinB == SET)
    {
        encoder_position++;
        // GPIO_WriteReverse(LED_PORT_GREEN, LED_PIN_GREEN);
    }
    else if (pinA == SET && pinB == RESET)
    {
        encoder_position--;
        // GPIO_WriteReverse(LED_PORT_RED, LED_PIN_RED);
    }
} */