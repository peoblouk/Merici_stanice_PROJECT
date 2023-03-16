#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm8s.h"

/**
 * @brief  Rotary Encoder configuaration
 */
typedef struct
{
    GPIO_Pin_TypeDef clk;
    GPIO_Pin_TypeDef ds;
    BitStatus curr_state;
    BitStatus prev_state;
    void (*onLeft)();
    void (*onRight)();
} Encoder_Config;

void Encoder_Init(Encoder_Config *config);

#endif