#include "stm8s.h"
#include "delay.h"
#include "Serial.h"

/**
 * @brief  Configure Serial monitor.
 * @param  BaudRate : Speed of serial monitor.
 * @retval None
 */
void Serial_begin(uint32_t BaudRate)
{
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2, ENABLE);
    UART2_Init(BaudRate, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);
    UART2_Cmd(ENABLE);
    delay_ms_2(50);
}

/**
 * @brief  Print string to serial monitor.
 * @param  message : message to write.
 * @retval None
 */
void Serial_print(char message[])
{
    char i = 0;

    while (message[i] != 0x00)
    {
        UART2_SendData8(message[i]);

        while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET)
            ;
        i++;
    }
}
