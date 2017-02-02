#include <msp430g2553.h>

// This tutorial is based on Argenox Tutorial about MSP430 UART

int main (void) {
	WDTCTL = WDTPW + WDTHOLD;
	
	// Setting the clock to 1Mhz
	DCOCTL = 0;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	
	/* Configuring the pins so we can multiplex in p1.1 rxd and p1.2 txd
	when we set both bits in pxsel as 11 we are setting the pins as peripheral secondary mode */
	P1DIR |= BIT6 + BIT0;
	P1SEL = BIT1 | BIT2;
	P1SEL2 = BIT1 | BIT2;
	
	// Configuring UART
	// In order to set the desired specifications: 9600 Baud Rate and 1Mhz we must look at the user guide the specified values to UCA0BR0 and UCA0BR1
	UCA0CTL1 |= UCSSEL_2;	// Sub Main Clock
	UCA0BR0 = 104;
	UCA0BR1 = 0; 	

	UCA0MCTL = UCBRS0;	// Modulation UCBRSx = 1
	
	// Taking UCA off the reset
	UCA0CTL1 &= ~UCSWRST;
	
	// Enable USCI A0 RX Interrupt
	IE2 |= UCA0RXIE;	
	__bis_SR_register(LPM0_bits + GIE); // Turning on all interrupts (including UART) and setting in Low Power Mode
					    
} 
	
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {

	while (!(IFG2 & UCA0TXIFG)); 	// USCI_A0 TX buffer ready?
	UCA0TXBUF = UCA0RXBUF;		// TX; RXed character
	P1OUT ^= (BIT0 + BIT6);		// If it receives the character, then it should XOR the state
	UC0IE |= UCA0TXIE;
}
	
	
/* To check if the UART is working we just open a new terminal and type:
$ echo 'a' -n > /dev/ttyACM0
Realize that /dev/ttyACM0 is where the MSP430 is plugged in my computer, you may check yours before giving the command.
The "-n" used in the command is to block the auto line space that the command echo gives*/
