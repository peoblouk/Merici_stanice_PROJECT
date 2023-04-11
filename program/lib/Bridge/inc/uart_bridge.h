#include "stm8s.h"

#define UART_BAUDRATE 9600

// PD5 UART TX

void uart_init(void)
{
    UART1_DeInit();
    UART1_Init(UART_BAUDRATE, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
               UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    UART1_Cmd(ENABLE);
}

void uart_send(uint8_t *data, uint8_t len)
{
    while (len--)
    {
        UART1_SendData8(*data++);
        while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
            ;
    }
}

/* uint8_t uart_recv(uint8_t *data, uint8_t len)
{
    uint8_t i = 0;

    while (len--)
    {
        while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET)
            ;
        *data++ = UART1_ReceiveData8();
        i++;
    }

    return i;
} */