/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 17-05-2022 - 18:48:22
 * @modify date 17-05-2022 - 18:48:22
 * @desc [Milis function]
 */

#ifndef _MILIS_H_
#define _MILIS_H_ 1
#include "stm8s_conf.h"

/* typedef struct
{
    uint16_t (*get)(void);
    void (*init)(void);
} Milis_module; */

// configure PRESCALER and PERIOD according to your clock, to setup 1ms time base
#if F_CPU == 16000000
// varianta pro 16MHz
#define PRESCALER TIM2_PRESCALER_128
#define PERIOD (125 - 1)
#elif F_CPU == 2000000
// varianta pro 2MHz
#define PRESCALER TIM2_PRESCALER_16
#define PERIOD (125 - 1)
#else
#warning "Milis timer parameters not defined for this clock !"
#endif
uint16_t get_milis(void);
void init_milis(void);
// extern const Milis_module milis;

#endif
/*****************************END OF FILE****/