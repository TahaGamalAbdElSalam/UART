/******************************************************************************
 * main.c
 *
 * Created on: September 3, 2019
 * Author: Taha
 ******************************************************************************/

#include"uart.h"

int main(void)
{
	uint8 data;
	UART_Init();
	UART_Start();
	while(1)
	{
		UART_Receive_Byte(&data); 
		UART_Send_Byte(data);
	}
}

