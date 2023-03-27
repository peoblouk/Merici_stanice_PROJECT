#include "rfid_uart.h"

void RFID_Begin()
{
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
    UART1_Init(9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    UART1_Cmd(ENABLE);
    UART1_SendData8(0xEB); // Dej vědět MFRC, že má bězět na 9600 kBit/s
    delay_ms_2(50);        //! TADY MŮŽE NASTAT PROBBLÉM podle logické analýzátoru je delay(50) = 63,17 ms
    // TODO TADY JE TŘEBA VYŘEŠIT tento delay
}

void RFID_write(char message[])
{
    char i = 0;

    while (message[i] != 0x00)
    {
        UART1_SendData8(message[i]);

        while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
            ;
        i++;
    }
}

char RFID_read()
{
    while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET)
        ;
    UART1_ClearFlag(UART1_FLAG_RXNE);

    return UART1_ReceiveData8();
}
