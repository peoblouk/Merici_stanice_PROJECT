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

// Funtion Declarations
void Serial_begin(uint32_t);      // pass baug rate and start serial communiaction
void Serial_print_int(int);       // pass integer value to print it on screen
void Serial_print_char(char);     // pass char value to print it on screen
void Serial_print_string(char[]); // pass string value to print it
void Serial_newline(void);        // move to next line
bool Serial_available(void);      // check if input serial data available return 1 is yes
char Serial_read_char(void);      // read the incoming char byte and return it

#endif /* __SERIAL_H  */
