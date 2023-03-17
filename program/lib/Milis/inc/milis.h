/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 17-05-2022 - 18:48:22
 * @modify date 17-05-2022 - 18:48:22
 * @desc [Milis function]
 */

#ifndef _MILIS_H_
#define _MILIS_H_ 1

#include "stm8s.h"

/* typedef struct
{
    uint16_t (*get)(void);
    void (*init)(void);
} Milis_module;
 */

// nastavení prescaleru podle frekvence procesoru
#if F_CPU == 16000000
// varianta pro 16MHz
#define PRESCALER TIM2_PRESCALER_128
#define PERIOD (124)
#elif F_CPU == 2000000
// varianta pro 2MHz
#define PRESCALER TIM2_PRESCALER_16
#define PERIOD (124)
#else
#warning "milis timer parameters not defined for this clock !"
#endif

uint16_t get_milis(void);
void init_milis(void);
// extern const Milis_module milis;

#endif