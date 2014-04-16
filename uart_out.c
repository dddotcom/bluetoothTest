/*
 * uart_out.c
 *
 *  Created on: Apr 16, 2014
 *      Author: ddortega
 */

#include <msp430g2553.h>

// to use the strlen function in the function tx_start_string below
#include <string.h>

#define SMCLKRATE 8000000
#define BAUDRATE 9600

#define BRDIV16 ((16*SMCLKRATE)/BAUDRATE)
#define BRDIV (BRDIV16/16)
#define BRMOD ((BRDIV16-(16*BRDIV)+1)/16)
#define BRDIVHI (BRDIV/256)
#define BRDIVLO (BRDIV-BRDIVHI*256)

#define TXBIT 0x04
//#define RXBIT 0x02

void init_USCI_UART() {
	UCA0CTL1 = UCSWRST; // reset and hold UART state machine
	UCA0CTL1 |= UCSSEL_2; // select SMCLK as the clock source
	UCA0BR1 = BRDIVHI; // set baud parameters, prescaler hi
	UCA0BR0 = BRDIVLO; // prescaler lo
	UCA0MCTL = 2 * BRMOD; // modulation
	// setup the TX pin (connect the P1.2 pin to the USCI)
	P1SEL |= TXBIT;
	P1SEL2 |= TXBIT;
	UCA0CTL1 &= ~UCSWRST; // allow the UART state machine to operate
}

char *tx_next_character; // pointer to the next character to transmit
int tx_count; // remaining number of characters to transmit

int tx_buffer_count() {
	return tx_count;
}

// UART Transmit interrupt handler:

void interrupt tx_handler() {
	if (tx_count > 0) { // are there characters left to transmit?
		--tx_count; // decrement the count
		UCA0TXBUF = *tx_next_character++; // send the current character & advance the pointer
	} else { // when no characters left
		IE2 &= ~UCA0TXIE; // disable the transmit interrupt
	}
}
ISR_VECTOR(tx_handler, ".int06")
// declare interrupt vector

int tx_start(char *buffer, int count) {
	if (tx_count == 0) { // check that a previous transmission is not still in progress
		tx_count = count; // store parameters for the transmit interrupt
		tx_next_character = buffer;
		IE2 |= UCA0TXIE; // enable the transmit interrupt (this will immediately generate the 1st interrupt)
		return 0; // success (transmission started)
	} else { // busy error
		return -1; // failure
	}
}

// Syntatic sugar to start transmitting a C string
int tx_start_string(char *s){
 return tx_start(s,strlen(s));
}

