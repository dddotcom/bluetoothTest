#include <msp430g2553.h>

#include "uart_out.h"

// include declarations for snprintf from the library & generic string stuff
#include <string.h>
#include <stdio.h>

volatile int latest_result; // most recent result is stored in latest_result

// a buffer to construct short text output strings
#define CBUFLEN 20
char cbuffer[CBUFLEN]; // buffer for output of characters
int init_cbuffer;


#define BUTTON_BIT 0x08
//#define TA0_BIT 0x02 //p1.4
#define initialHalfPeriod 500


void init_button(void); // routine to setup the button

void main() {

	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
	BCSCTL1 = CALBC1_8MHZ; // 8Mhz calibration for clock
	DCOCTL = CALDCO_8MHZ;
	 P1DIR |= 0x01;

	init_USCI_UART(); // initialize the UART

	init_button(); // initialize the button

	// Setup printing
//	 conPrintCounter=conPrint;
//	 conPrintSequence=1;
	_bis_SR_register(GIE+LPM0_bits);// enable general interrupts and power down CPU

	 //u_print_string("Printing every ");
	 //snprintf(cbuffer,CBUFLEN,"%d",conPrint);
	 //u_print_string(cbuffer);
	 //u_print_string(" steps\r\n N Value\r\n");

}




void init_button(){
// All GPIO's are already inputs if we are coming in after a reset
	P1OUT |= BUTTON_BIT; // pullup
	P1REN |= BUTTON_BIT; // enable resistor
	P1IES |= BUTTON_BIT; // set for 1->0 transition
	P1IFG &= ~BUTTON_BIT;// clear interrupt flag
	P1IE  |= BUTTON_BIT; // enable interrupt
	init_cbuffer = 10;
}

void interrupt button_handler(){
// check that this is the correct interrupt
// (if not, it is an error, but there is no error handler)
	if (P1IFG & BUTTON_BIT){
		P1IFG &= ~BUTTON_BIT; // reset the interrupt flag
		init_cbuffer = init_cbuffer + 10;
		snprintf(cbuffer,CBUFLEN,"%dX",init_cbuffer);
		tx_start_string(cbuffer);
		 P1OUT ^= 1;
		//TACCTL0 ^= OUTMOD_4; // toggle outmod between 0 and 4 (toggle)
	}
}
ISR_VECTOR(button_handler,".int02") // declare interrupt vector
