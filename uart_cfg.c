/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart_cfg.c
 *
 * Description: Source file for configurations of the UART AVR driver
 *
 *  Created on: September 3, 2019
 *      Author: Taha
 ******************************************************************************/

/*******************************************************************************
 *                       	Included Libraries                                 *
 *******************************************************************************/
#include"uart_cfg.h"
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_CAPT_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

UART_cfg UART_Config =
{
		9600,
		UART_DoubleSpeedEn,
		UART_TxIntDisabled,
		UART_RxIntDisabled,
		UART_UdrDisabled,
		Bit8,
		UART_ParityDisabled,
		UART_StopBit1
};



void Icu_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

