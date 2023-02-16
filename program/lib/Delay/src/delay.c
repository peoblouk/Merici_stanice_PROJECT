/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 31-05-2022 - 17:05:15
 * @modify date 31-05-2022 - 17:05:15
 * @desc [Delay Library]
 */

#include "delay.h"

void delay_init(void) // inicialization of counter
{
	TIM3_DeInit();
	TIM3_TimeBaseInit(TIM3_PRESCALER_64, 249); // 16 000 000 / (1000 * 64) = 1 ms
	TIM3_Cmd(ENABLE);
}

void delay_reset(void) // helping fuction for reseting timer
{
	TIM3_SetCounter(0);
}

void delay_cycles(uint32_t value) // function for delay created by loop
{
	uint32_t i;
	for (i = 0; i < value; i++)
		;
}

void delay_ms(uint32_t time_ms) // function for delay created by timer
{
	delay_reset();
	for (uint32_t loop_count = 0; loop_count < time_ms; loop_count++)
	{
		while (TIM3_GetFlagStatus(TIM3_FLAG_UPDATE) != SET)
			; // wait until flag change
		TIM3_ClearFlag(TIM3_FLAG_UPDATE);
	}
}

void delay_us(uint32_t time_us) // function for delay created by timer
{
	delay_reset();
	for (uint32_t loop_count = 0; loop_count < (time_us / 1000); loop_count++)
	{
		while (TIM3_GetFlagStatus(TIM3_FLAG_UPDATE) != SET)
			;
		TIM3_ClearFlag(TIM3_FLAG_UPDATE);
	}
}

const Delay_Module delay = {
	.init = delay_init,
	.cycles = delay_cycles,
	.ms = delay_ms,
	.us = delay_us,
};

void delay_ms_2(uint32_t ms)
{
    uint32_t clock_freq = CLK_GetClockFreq();

    for (uint32_t i = 0; i < ((clock_freq / 18 / 1000UL) * ms); i++)
    {
        nop();
    }
}

void delay_us_2(uint32_t us)
{
    uint32_t clock_freq = CLK_GetClockFreq();

    for (uint32_t i = 0; i < ((clock_freq / 18 / 1000UL) * (us / 1000)); i++)
    {
        nop();
    }
}