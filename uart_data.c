///*
// * uart_data.c
// *
// *  Created on: Feb 11, 2026
// *      Author: luceng
// */
//
//#include <stdio.h>
//#include <stdarg.h>
//#include "uart_data.h"
//
//void cyBot_sendString(const char *str) {
//    int i;
//	for (i = 0; str[i] != 0; ++i) {
//		cyBot_sendByte(str[i]);
//	}
//	// don't forget null byte
//	cyBot_sendByte(0);
//}
//
//int cyBot_sendf(const char *format, ...) {
//	va_list args;
//	va_start(args, format);
//
//	char *str = NULL;
//	vasprintf(&str, format, args);
//	if (!str) {
//		// allocation failure
//		return -1;
//	}
//	cyBot_sendString(str);
//	free(str);
//	return 0;
//}
