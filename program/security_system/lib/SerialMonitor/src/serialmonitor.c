/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 22-12-2022 - 13:23:22
 * @modify date 22-12-2022 - 13:23:22
 * @desc [Serial Monitor library]
 */

#include "serialmonitor.h"

#define PIN_TX GPIO_PIN_5
#define PIN_RX GPIO_PIN_6

char Serial_read_char() //! Function to read char from serial
{
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
        ;
    UART1_ClearFlag(UART1_FLAG_RXNE);
    return (UART1_ReceiveData8());
}

void Serial_print_char(char value) //! Function for printing char
{
    UART1_SendData8(value);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
        ;
}

void Serial_begin(uint32_t baud_rate) //! Function to initialize serial monitor and set baudrate
{
    GPIO_Init(GPIOD, PIN_TX, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOD, PIN_RX, GPIO_MODE_IN_PU_NO_IT);

    UART1_DeInit(); // Deinitialize UART peripherals

    UART1_Init(baud_rate,
               UART1_WORDLENGTH_8D,
               UART1_STOPBITS_1,
               UART1_PARITY_NO,
               UART1_SYNCMODE_CLOCK_DISABLE,
               UART1_MODE_TXRX_ENABLE); // (BaudRate, Wordlegth, StopBits, Parity, SyncMode, Mode)
    UART1_Cmd(ENABLE);
}

void Serial_print_int(int number) //! Funtion to print int value to serial monitor
{
    char count = 0;
    char digit[5] = "";

    while (number != 0) // split the int to char array
    {
        digit[count] = number % 10;
        count++;
        number = number / 10;
    }

    while (count != 0) // print char array in correct direction
    {
        UART1_SendData8(digit[count - 1] + 0x30);
        while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
            ;
        count--;
    }
}

void Serial_newline() //! Funtion to print newline to serial monitor
{
    UART1_SendData8(0x0a);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
        ;
}

void Serial_print_string(char string[]) //! Function to print string in serial monitor
{

    char i = 0;

    while (string[i] != 0x00)
    {
        UART1_SendData8(string[i]);
        while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
            ;
        i++;
    }
}

bool Serial_check()
{
    if (UART1_GetFlagStatus(UART1_FLAG_RXNE) == TRUE)
        return TRUE;
    else
        return FALSE;
}

const Serialmonitor_Module Serial = {
    .begin = Serial_begin,
    .check = Serial_check,
    .print_int = Serial_print_int,
    .print_char = Serial_print_char,
    .print_str = Serial_print_string,
    .new_line = Serial_newline,
    .read = Serial_read_char,
};