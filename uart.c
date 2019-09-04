/*
 * uart.c
 *
 *  Created on: September 3, 2019
 *      Author: Taha
 */

#include"uart.h"
#include <stddef.h>
#include <avr/interrupt.h>

volatile uint8 data_global = 0;
static volatile uint8 flag = 0;
static volatile uint8 g_flag   = LOW;
static volatile uint8 g_index  = LOW;

ISR(USART_UDRE_vect)
{
	if(g_flag != LOW)
	{
		UDR = g_flag;
		g_index++ ;
	}
}

/* ISR for receive vector */
ISR(USART_RXC_vect)
{
	flag = 1;
}


/*******************************************************************************
 * Function Name:	UART_Init
 *
 * Description: 	*Initialize the UART Driver Registers
 * 					*Set the driver Configurations
 *
 * Inputs:			Configuration Structure
 *
 * Outputs:			NULL
 *
 * Return:			Status to check function execution
 *******************************************************************************/
Status UART_Init(void)
{
	uint16 UBRR = 0u;
	if(UART_Config.DoubleSpeed == UART_DoubleSpeedDisable)
	{
		CLEAR_BIT(UCSRA,U2X);
		UBRR = (((F_CPU / (UART_Config.baud_rate * 16UL))) - 1);
	}
	else if(UART_Config.DoubleSpeed == UART_DoubleSpeedEn)
	{
		SET_BIT(UCSRA,U2X);
		UBRR = (((F_CPU / (UART_Config.baud_rate * 8UL))) - 1);
	}
	else
	{
		return NotOk;
	}

	/*clear to choose UBRRH*/
	CLEAR_BIT(UCSRC,URSEL);
	UBRRL = (uint8)UBRR;
	UBRRH = (uint8)(UBRR >> 8);
	SET_BIT(UCSRC,URSEL);
	/*set to choose UCSRC*/
	
	/* DataSize config */
	/********************************************************************************************/
	if(UART_Config.DataSize == Bit5)
	{
		CLEAR_BIT(UCSRB,UCSZ2);
		CLEAR_BIT(UCSRC,UCSZ1);
		CLEAR_BIT(UCSRC,UCSZ0);
	}
	else if(UART_Config.DataSize == Bit6)
	{
		CLEAR_BIT(UCSRB,UCSZ2);
		CLEAR_BIT(UCSRC,UCSZ1);
		SET_BIT(UCSRC,UCSZ0);
	}
	else if(UART_Config.DataSize == Bit7)
	{
		CLEAR_BIT(UCSRB,UCSZ2);
		SET_BIT(UCSRC,UCSZ1);
		CLEAR_BIT(UCSRC,UCSZ0);
	}
	else if(UART_Config.DataSize == Bit8)
	{
		CLEAR_BIT(UCSRB,UCSZ2);
		SET_BIT(UCSRC,UCSZ1);
		SET_BIT(UCSRC,UCSZ0);
	}
	else if(UART_Config.DataSize == Bit9)
	{
		SET_BIT(UCSRB,UCSZ2);
		SET_BIT(UCSRC,UCSZ1);
		SET_BIT(UCSRC,UCSZ0);
	}
	else
	{
		return NotOk;
	}

	/* Parity config */
	/*********************************************************************************/
	if(UART_Config.Parity == UART_ParityDisabled)
	{
		CLEAR_BIT(UCSRC,UPM0);
		CLEAR_BIT(UCSRC,UPM1);
	}
	else if(UART_Config.Parity == UART_ParityOdd)
	{
		SET_BIT(UCSRC,UPM0);
		SET_BIT(UCSRC,UPM1);
	}
	else if(UART_Config.Parity == UART_ParityEven)
	{
		CLEAR_BIT(UCSRC,UPM0);
		SET_BIT(UCSRC,UPM1);
	}
	else
	{
		return NotOk;
	}

	/* Stop_bit config */
	/*****************************************************************************/
	if(UART_Config.StopBits == UART_StopBit1)
	{
		CLEAR_BIT(UCSRC,USBS);
	}
	else if(UART_Config.StopBits == UART_StopBit2)
	{
		SET_BIT(UCSRC,USBS);
	}
	return Ok;
}

/*******************************************************************************
 * Function Name:	UART_Send_Byte
 *
 * Description: 	transmit a new byte
 *
 * Inputs:			Transmitting Byte
 *
 * Outputs:			NULL
 *
 * Return:			Status to check function execution
 *******************************************************************************/

Status UART_Send_Byte(const uint8 data)
{

	if(UART_Config.Udr  == UART_UdrDisabled)
	{
		/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for
		 * transmitting a new byte so wait until this flag is set to one */

		while ( !(UCSRA & (1<<UDR)) );

		/* Put the required data in the UDR register and it also clear the UDRE flag as
		 * the UDR register is not empty now */

		UDR = data;
	}
	else if(UART_Config.Udr == UART_UdrEn)
	{
		g_flag = data;
	}
	else
	{
		return NotOk;
	}
	return Ok;
}
/*******************************************************************************
 * Function Name:	UART_Receive_Byte
 *
 * Description: 	*used to receive a byte
 * 					
 *
 * Inputs:			None
 *
 * Outputs:			received data
 *
 * Return:			Status to check function execution
 *******************************************************************************/
Status UART_Receive_Byte(uint8* data){
	if (UART_Config.RxInt == UART_RxIntDisabled)
	{
		/* Wait for data to be received */
		while ( !(UCSRA & (1<<RXC)) );
		/* Receive data from receiver buffer */
		*data = UDR;
	}
	else if(UART_Config.RxInt == UART_RxIntEn)
	{
		/* ISR will be executed */	
		if (flag == 1)
		{
			*data = UDR;
			flag = 0;
		}
		else
		{
			/* To avoid garbage value */
			*data = NULL;
		}
	}
	else
	{
		return NotOk;
	}
	
	return Ok;
}

/*******************************************************************************
 * Function Name:	UART_Send_String
 *
 * Description: 	transmit a new String
 *
 * Inputs:			Transmitted String
 *
 * Outputs:			NULL
 *
 * Return:			Status to check function execution
 *
 * Notes:			you should add # after your string to transmit it
 * 					your string will not be transmitted if you don't add #.
 *******************************************************************************/

Status UART_Send_String(const uint8 * data)
{
	uint8 index = 0u;
	if(UART_Config.Udr  == UART_UdrDisabled)
	{
		while(data[index] != '\0')
		{
			UART_Send_Byte(data[index]);
			index++;
		}
	}
	else if(UART_Config.Udr == UART_UdrEn)
	{
		g_index = 0u;
		while(data[g_index] != '\0')
		{
			UART_Send_Byte(data[g_index]);
		}
	}
	else
	{
		return NotOk;
	}

	return Ok;
}

/*******************************************************************************
 * Function Name:	UART_Receive_String
 *
 * Description: 	*used to receive string of Bytes
 * 					
 *
 * Inputs:			None
 *
 * Outputs:			String of received data
 *
 * Return:			Status to check function execution
 *******************************************************************************/
Status UART_Receive_String(uint8 * data){
	static uint8 counter_looper = 0;

	if (UART_Config.RxInt == UART_RxIntDisabled)
	{
		while(UDR != '#'){
			UART_Receive_Byte(&data[counter_looper]);
			counter_looper++;
		}
		counter_looper=0;
		
	}
	else if(UART_Config.RxInt == UART_RxIntEn)
	{
		/* ISR will be executed */
		if(UDR != '#'){
			UART_Receive_Byte(&data[counter_looper]);
			/* Garbage value detection */
			if (&data[counter_looper] != NULL)
			{
				counter_looper++;
			}
		}
		else
		{
			counter_looper=0;
		}
	}
	else
	{
		return NotOk;
	}

	
	return Ok;
}
/*******************************************************************************/
//void UART_Receive_Byte_Callback(void)
//{
	//static uint8 global_counter=0;
	//recieved_string[global_counter]=UDR;
	//global_counter++;
	//if(recieved_string[global_counter]=='#')
	//{
		//global_counter=0;
	//}
//}
//void setcallback(&UART_Receive_Byte_Callback)
//{
	//ptr_func=&UART_Receive_Byte_Callback;
//}
/******************************************************************************/

/*******************************************************************************
 * Function Name:	UART_Stop
 *
 * Description: 	*Used to stop UART module with all it's interrupt
 * 					
 *
 * Inputs:			None
 *
 * Outputs:			None
 *
 * Return:			Status to check function execution
 *******************************************************************************/
Status UART_Stop(void){

	CLEAR_BIT(UCSRB,TXEN);
	CLEAR_BIT(UCSRB,RXEN);
	CLEAR_BIT(UCSRB,RXCIE);
	CLEAR_BIT(UCSRB,TXCIE);
	CLEAR_BIT(UCSRB,UDRIE);
	return Ok;
}

/*******************************************************************************
 * Function Name:	UART_Start
 *
 * Description: 	*Used to start UART Tx and Rx in both ISR or polling techniqe
 * 					
 *
 * Inputs:			None
 *
 * Outputs:			None
 *
 * Return:			Status to check function execution
 *******************************************************************************/
Status UART_Start(void){

	/* Enable Tx and Rx module */
	UCSRB |= (1<<TXEN)|(1<<RXEN);
	
	/**************************************************************************/
	if(UART_Config.TxInt  == UART_TxIntDisabled)
	{
		CLEAR_BIT(UCSRB,TXCIE);
	}
	else if(UART_Config.TxInt  == UART_TxIntEn)
	{
		SET_BIT(UCSRB,TXCIE);
	}
	else
	{
		return NotOk;
	}
	/*************************************************************************************/
	if(UART_Config.RxInt  == UART_RxIntDisabled)
	{
		CLEAR_BIT(UCSRB,RXCIE); 
	}
	else if(UART_Config.RxInt  == UART_RxIntEn)
	{
		SET_BIT(UCSRB,RXCIE);
	}
	else
	{
		return NotOk;
	}
	/***************************************************************************************/
	if(UART_Config.Udr == UART_UdrDisabled)
	{
		CLEAR_BIT(UCSRB,UDRIE);
	}
	else if(UART_Config.Udr  == UART_UdrEn)
	{
		SET_BIT(UCSRB,UDRIE);
	}
	else
	{
		return NotOk;
	}

	return Ok;
}
