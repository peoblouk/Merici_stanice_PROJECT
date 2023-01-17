/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 22-12-2022 - 13:23:22
 * @modify date 22-12-2022 - 13:23:22
 * @desc [Serial Monitor]
 */

#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm8s.h"
#include <stdio.h>

typedef struct
{
    void (*begin)(uint32_t);
    void (*print_int)(int);
    void (*print_char)(char);
    void (*print_str)(char[]);
    void (*new_line)(void);
    bool (*check)(void);
    char (*read)(void);
} Serialmonitor_Module;

// Funtion Declarations
void Serial_begin(uint32_t);      // baud rate and start serial communication
void Serial_print_int(int);       // integer value to print it on screen
void Serial_print_char(char);     // char value to print it on screen
void Serial_print_string(char[]); // string value to print it
void Serial_newline(void);        // move to next line
bool Serial_check(void);          // check if input serial data available return 1 is yes
char Serial_read_char(void);

extern const Serialmonitor_Module Serial;

#endif /* __SERIAL_H  */
