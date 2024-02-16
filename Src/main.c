/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Mateusz Salamon
 * @brief          : STM32 na Rejestrach
 ******************************************************************************
 ******************************************************************************

 	 	 	 	 https://msalamon.pl
 	 	 	 	 https://sklep.msalamon.pl
 	 	 	 	 https://kursstm32.pl
 	 	 	 	 https://stm32narejestrach.pl

 */
#include "main.h"

// Constants for Software Timer's actions
#define UART2_TIMER 500

// LED LD4 macros
#define LD4_ON GPIOA->BSRR = GPIO_BSRR_BS5
#define LD4_OFF GPIOA->BSRR = GPIO_BSRR_BR5;

// Tick for System Time
__IO uint32_t Tick;

// LED LD4 configuration
void LD4_Configure(void);

// UART Configuration
void UART2_Config(void);
void UART2_SendChar(char ch);
void UART2_SendString(char *str);
uint8_t UART2_GetChar(char *ch);

// Get current System Time
uint32_t GetSystemTick(void);

// Software Timers variables
uint32_t Timer_UART2;

int main(void)
{
// 1s = 12 000 000
	// 0,001 = X
	SysTick_Config(12000000 / 1000);

// Configure LD4
	LD4_Configure();

	// Configure UART
	UART2_Config();

	// Software Timers - first fill
	Timer_UART2 = GetSystemTick();

	char ReceiveTmp;

    /* Loop forever */
	while(1)
	{

//		// UART2 transmit
//		if((GetSystemTick() - Timer_UART2) > UART2_TIMER)
//		{
//			Timer_UART2 = GetSystemTick();
//
//			// Put data to transmit register
//			USART2->TDR = 'A';
//
//			// ISR TXFNF
//			while(!(USART2->ISR & USART_ISR_TXE_TXFNF))
//			{
//				// Do nothing, wait for end of transmisssion
//			}
//		}

		// UART2 Receive

//		while(!(USART2->ISR & USART_ISR_RXNE_RXFNE))
//		{
//			// Data NOT received
//		}
//			// Data received
//		ReceiveTmp = USART2->RDR;
//
//		if(ReceiveTmp == '1')
//		{
//			LD4_ON;
//		}
//		else if(ReceiveTmp == '0')
//		{
//			LD4_OFF;
//		}

//		if(USART2->ISR & USART_ISR_RXNE_RXFNE)
//		{
//			// Data received
//			ReceiveTmp = USART2->RDR;
//
//			if(ReceiveTmp == '1')
//			{
//				LD4_ON;
//			}
//			else if(ReceiveTmp == '0')
//			{
//				LD4_OFF;
//			}
//		}

		// UART2 transmit
		if((GetSystemTick() - Timer_UART2) > UART2_TIMER)
		{
			Timer_UART2 = GetSystemTick();

//			UART2_SendChar('A');
			UART2_SendString("STM32 na Rejestrach\n\r");
		}

		uint8_t ReceiveStatus = UART2_GetChar(&ReceiveTmp);

		if(ReceiveStatus == 0)
		{
			// char received
			if(ReceiveTmp == '1')
			{
				LD4_ON;
			}
			else if(ReceiveTmp == '0')
			{
				LD4_OFF;
			}
		}
		else
		{
			// char NOT received
		}

	}
}


// PA2 - TX PA3 - RX

void UART2_Config(void)
{
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Pin PA2 - TX AF Mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE2_0);
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0;

	// Pin PA3 - RX AF Mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE3_0);
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0;
	GPIOA->OTYPER |= GPIO_OTYPER_OT3;

	// Enable USART2 Clock
	RCC->APBENR1 |= RCC_APBENR1_USART2EN;

	// 115200 baudrate
	USART2->BRR = 12000000/115200;

//	USART2->CR1 |= USART_CR1_UE; // UART Enable
//	USART2->CR1 |= USART_CR1_TE; // Transmitter Enable`
//	USART2->CR1 |= USART_CR1_RE; // Receiver Enable

	USART2->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}


void UART2_SendString(char *str)
{
	while(*str != '\0') // If current char is not c-string endchar
	{
		UART2_SendChar(*str); // Send char by UART
		str++; // Jump to next char in c-string
	}
}

void UART2_SendChar(char ch)
{
	// UART2 transmit

	// Put data to transmit register
	USART2->TDR = ch;

	// ISR TXFNF
	while(!(USART2->ISR & USART_ISR_TXE_TXFNF))
	{
		// Do nothing, wait for end of transmisssion
	}
}

//void UART2_GetChar(char *ch)
uint8_t UART2_GetChar(char *ch)
{
	if(USART2->ISR & USART_ISR_RXNE_RXFNE)
	{
		// Data received
		*ch = USART2->RDR;

		return 0;
	}

	// Data NOT received
	return 1;
}

void LD4_Configure(void)
{
	// Enable Clock for PORTD
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Configure GPIO Mode - Output
	GPIOA->MODER |= GPIO_MODER_MODE5_0; // It's default reset state. Not necessary.
	GPIOA->MODER &= ~(GPIO_MODER_MODE5_1);

	// Configure Output Mode - Push-pull
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5); // It's default reset state. Not necessary.

	// Configure GPIO Speed - Low
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5); // Two bits together. It's default reset state. Not necessary.

	// Configure Pull-up/Pull-down - no PU/PD
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5); // It's default reset state. Not necessary.
}

void SysTick_Handler(void)
{
	Tick++; // Increase system timer
}

uint32_t GetSystemTick(void)
{
	return Tick; // Return current System Time
}

