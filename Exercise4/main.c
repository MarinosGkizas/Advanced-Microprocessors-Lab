/*
 * Ask4.c
 *
 * Created: 21/5/2021 9:24:15 μμ
 * Author : Marinos
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(PORTF_PORT_vect ){
	//clear the interrupt flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS=intflags;
	int random=rand();
	
}
ISR(TCA0_CMP0_vect){
//clear the interrupt flag
int intflags = TCA0.SINGLE.INTFLAGS;
TCA0.SINGLE.INTFLAGS = intflags;
//Κώδικας για αποθήκευση σε array του random

}

ISR(TCB0_INT_vect){
	//clear the interrupt flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;
	//Κώδικας για εμφάνιση της τιμής στα le0-led3
}

ISR(PORTE_PORT_vect ){
	//clear the interrupt flag
	int intflags = PORTE.INTFLAGS;
	PORTE.INTFLAGS=intflags;
	//κώδικας για διάβασμα του array της τελευταιας τιμής που αποθηκέυτηκε
	
}


int main(){
	PORTD.DIR |= PIN0_bm; //PIN is output
	PORTD.DIR |= PIN1_bm; //PIN is output
	PORTD.DIR |= PIN2_bm; //PIN is output
	PORTD.DIR |= PIN3_bm; //PIN is output
	
	//pullup enable and Interrupt enabled with sense on both edges
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	PORTE.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_CMP0_bm;
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; //Enable
	
	sei(); //Enable interrupts
	//prescaler=1024
	TCA0.SINGLE.CTRLA=TCA_SINGLE_CLKSEL_DIV1024_gc;
	TCA0.SINGLE.PER = 54; //select the resolution
	TCA0.SINGLE.CMP0 = 27; //select the duty cycle
	//select Single_Slope_PWM
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	//enable interrupt Overflow
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	//enable interrupt COMP0
	
	//Το ίδιο και για τον δεύτερο timer του TCB0 αλλα με διαφορετικο resolution,duty cycle
	
	while (1){ }
	cli(); //Disenable interrupts
}

