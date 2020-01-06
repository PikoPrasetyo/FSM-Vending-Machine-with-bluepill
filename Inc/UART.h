/*
 * UART.h
 *
 *  Created on: Nov 23, 2019
 *      Author: Piko Prasetyo
 */

#ifndef UART_H_
#define UART_H_

#include "string.h"
#include "stdio.h"
#include "main.h"

void UART_Print(char *pData);
void UART_Receive (char *pData);

#endif /* UART_H_ */
