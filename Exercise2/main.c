#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ped 10
int upper=99,lower=0;
int interrupt=0;

ISR(TCA0_CMP0_vect)
{
	TCA0.SINGLE.CTRLA = 0; //Disable
	TCA0.SINGLE.CNT = 0; //timer counter to zero
	//clear flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS=intflags;
	PORTD.OUTSET =PIN2_bm; // RED
	interrupt=0;
	
}

ISR(PORTF_PORT_vect) //Πάτημα κουμπιού για πεζούς
{
	//clear the interrupt flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS=intflags;
	PORTD.OUTCLR=PIN2_bm; // Φανάρι πεζών GREEN
	PORTD.OUTCLR=PIN1_bm; // Φανάρι μικρού δρόμου GREEN
	PORTD.OUTSET=PIN0_bm; // Φανάρι μεγάλου δρόμου RED
	TCA0.SINGLE.CTRLA |=1;//Enable timer
	interrupt=0;
	


}


int main(void){

	// pin 0--> Φανάρι μικρού δρόμου
	// pin 1--> Φανάρι μεγάλου δρόμου
	// pin 2-> Φανάρι πεζών
	srand(time(NULL));

	PORTD.DIR |= PIN0_bm; //PIN 0 is output
	PORTD.DIR |= PIN1_bm; //PIN 1 is output
	PORTD.DIR |= PIN2_bm; //PIN 2 is output
	PORTD.OUT |= PIN0_bm; //LED είναι off για τον μικρό δρόμο
	PORTD.OUTCLR = PIN1_bm; //LED is on for big road
	PORTD.OUT |= PIN2_bm; //LED είναι off για τον μικρό δρόμο
	TCA0.SINGLE.CNT = 0; //clear counter
	TCA0.SINGLE.CTRLB = 0; //Normal Mode 	(TCA_SINGLE_WGMODE_NORMAL_gc)
	TCA0.SINGLE.CMP0 = ped; //When reaches this value -> 	interrupt CLOCK FREQUENCY/1024
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc; //(= 	0x7<<1)
	TCA0.SINGLE.CTRLA |=1;//Enable
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; //Interrupt 	Enable (=0x10)
	sei(); //begin accepting interrupt signals
		
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | 	PORT_ISC_BOTHEDGES_gc;
	
	while (interrupt==0) {
		int random = rand() %10; // Παίρνω έναν random αριθμό
		
		if ( random ==0 || random ==5|| random ==8 )

		{
			PORTD.OUT |= PIN1_bm; //Led1 είναι off για τον μεγάλο δρόμο
			PORTD.OUTCLR = PIN0_bm; //Led0 είναι on για τον μικρό δρόμο
		}
		else
		{
			
			PORTD.OUT |= PIN0_bm; //Led0 είναι off για τον μικρό δρόμο
			PORTD.OUTCLR = PIN1_bm; //Led1 είναι on για τον μεγάλο δρόμο
			
		}
	}
	
	cli();
}

