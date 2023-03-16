
#include "encoder.h"
#include "delay.h"
#include "Serial.h"

static Encoder_Config *encoder_config;

INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 5)
{
    encoder_config->curr_state = GPIO_ReadInputPin(GPIOD, encoder_config->clk);
    // encoder_config->btn_state = GPIO_ReadInputPin(GPIOD, encoder_config->btn);

    if (encoder_config->curr_state != encoder_config->prev_state && !encoder_config->curr_state)
    {
        if (GPIO_ReadInputPin(GPIOD, encoder_config->ds) != encoder_config->curr_state)
        {
            encoder_config->onLeft();
        }
        else
        {
            encoder_config->onRight();
        }
    }

    // else if (!encoder_config->btn_state)
    // {
    //     encoder_config->onClick();
    // }

    encoder_config->prev_state = encoder_config->curr_state;

    delay_ms(1);
}

void Encoder_Init(Encoder_Config *config)
{
    encoder_config = config;

    GPIO_Init(GPIOD, encoder_config->clk, GPIO_MODE_IN_PU_IT);
    GPIO_Init(GPIOD, encoder_config->ds, GPIO_MODE_IN_PU_IT);
    // GPIO_Init(GPIOD, encoder_config->btn, GPIO_MODE_IN_PU_IT);
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL);
    ITC_SetSoftwarePriority(ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_1);
    enableInterrupts();

    encoder_config->prev_state = GPIO_ReadInputPin(GPIOD, encoder_config->clk);
}