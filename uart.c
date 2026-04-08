///*
// *
// *   uart.c
// *
// *
// *
// *   @author
// *   @date
// */
//
//#include <inc/tm4c123gh6pm.h>
//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdarg.h>
//#include "uart.h"
//
//#include <stdbool.h>
//#include "driverlib/interrupt.h"
//
//#define REPLACEME 0
//
//void uart_init(void) {
//	//enable clock to GPIO port B
//	SYSCTL_RCGCGPIO_R |= 0b000010;
//
//	//enable clock to UART1
//	SYSCTL_RCGCUART_R |= 0b000010;
//
//	//wait for GPIOB and UART1 peripherals to be ready
//	while ((SYSCTL_PRGPIO_R & 0b000010) == 0)
//		;
//	while ((SYSCTL_PRUART_R & 0b000010) == 0)
//		;
//
//	//enable alternate functions on port B pins
//	GPIO_PORTB_AFSEL_R |= 0b000011;
//
//	//enable digital functionality on port B pins
//	GPIO_PORTB_DEN_R |= 0b000011;
//
//	//enable UART1 Rx and Tx on port B pins
//	GPIO_PORTB_PCTL_R &= ~0x0000ee;
//	GPIO_PORTB_PCTL_R |= 0x000011;
//
//#define BAUD_RATE 115200.0
//#define CLOCK_DIV 16.0
//#define BRD (16000000.0 / (CLOCK_DIV * BAUD_RATE))
//
//	//calculate baud rate
//	uint16_t iBRD = (uint16_t)BRD; //use equations
//	uint16_t fBRD = (uint16_t)((BRD - iBRD) * 64 + 0.5); //use equations
//
//	//turn off UART1 while setting it up
//	UART1_CTL_R &= ~0b01;
//
//	//set baud rate
//	//note: to take effect, there must be a write to LCRH after these assignments
//	UART1_IBRD_R = iBRD;
//	UART1_FBRD_R = fBRD;
//
//	//set frame, 8 data bits, 1 stop bit, no parity, no FIFO
//	//note: this write to LCRH must be after the BRD assignments
//	UART1_LCRH_R = 0b01100000;
//
//	//use system clock as source
//	//note from the datasheet UARTCCC register description:
//	//field is 0 (system clock) by default on reset
//	//Good to be explicit in your code
//	UART1_CC_R = 0x0;
//
//	//re-enable UART1 and also enable RX, TX (three bits)
//	//note from the datasheet UARTCTL register description:
//	//RX and TX are enabled by default on reset
//	//Good to be explicit in your code
//	//Be careful to not clear RX and TX enable bits
//	//(either preserve if already set or set them)
//	UART1_CTL_R |= 0b1100000001;
//
////	UART1_ICR_R |= 0b00010000;
////	UART1_IM_R |= 0b0110000;
////	NVIC_PRI1_R = (NVIC_PRI1_R & 0xFF0FFFF) | 0x00200000;
////	NVIC_EN0_R |= 0b0110000;
//}
//
//void uart_sendChar(char data) {
//	if (data == '\n') {
//		while (UART1_FR_R & 0x08)
//			;
//		UART1_DR_R = '\r';
//	}
//	else if (data == '\r') {
//        while (UART1_FR_R & 0x08)
//            ;
//        UART1_DR_R = '\n';
//    }
//	while (UART1_FR_R & 0x08)
//		;
//	UART1_DR_R = data;
//}
//
//char uart_receive(void) {
//	while (UART1_FR_R & 0x10)
//		;
//
//	return UART1_DR_R;
//}
//
//char uart_recieve_nonblocking(void) {
//	if (UART1_FR_R & 0x10) return UART1_DR_R;
//	else return 0;
//}
//
//void uart_sendStr(const char *str) {
//	int i;
//	for (i = 0; str[i] != 0; ++i) {
//		uart_sendChar(str[i]);
//	}
//	// don't forget null byte
//	uart_sendChar(0);
//}
//
//int uart_printf(const char *format, ...) {
//	va_list args;
//	va_start(args, format);
//	return uart_vprintf(format, args);
//}
//
//int uart_vprintf(const char *format, va_list args) {
//	char *str;
//	vasprintf(&str, format, args);
//	if (!str) {
//		// allocation failure
//		return -1;
//	}
//	uart_sendStr(str);
//	free(str);
//    va_end(args);
//	return 0;
//}
