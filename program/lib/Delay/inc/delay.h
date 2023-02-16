/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 31-05-2022 - 17:05:15
 * @modify date 31-05-2022 - 17:05:15
 * @desc [Delay Library]
 */

#ifndef DELAY_H
#define DELAY_H

#include "stm8s.h"
#include <stdio.h>

typedef struct
{
	void (*init)(void);
	void (*cycles)(uint32_t);
	void (*ms)(uint32_t);
	void (*us)(uint32_t);
} Delay_Module;

void delay_init(void);
void delay_reset(void);
void delay_cycles(uint32_t value);
void delay_ms(uint32_t time_ms);
void delay_us(uint32_t time_us);

void delay_us_2(uint32_t us);
void delay_ms_2(uint32_t ms);

extern const Delay_Module delay;

#endif /* DELAY_H */