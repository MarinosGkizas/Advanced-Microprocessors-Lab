/*
 * Askisi5.c
 *
 * Created: 4/6/2021 8:41:10 μμ
 * Author : Marinos
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ped 150

ISR(ADC0_WCOMP_vect){
	int intflags = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intflags;
	PORTD.OUTCLR= PIN2_bm; //LED is on
	
}
ISR(TCA0_CMP0_vect){
	//clear the interrupt flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;
	PORTD.OUT |= PIN1_bm //LED is on
	//μετα απο 1 duty cycle
	PORTD.OUTCLR |= PIN1_bm; //LED is off
	
}

int main(){
	PORTD.DIR |= PIN0_bm; //PIN is output θερμοσίφωνας
	PORTD.DIR |= PIN1_bm; //PIN is output ανεμιστήρας
	PORTD.DIR |= PIN2_bm; //PIN is output αισθητήρας water leakage
	PORTD.DIR |= PIN3_bm; //PIN is output κλειδωμα σπιτιου
	
	//initialize the ADC for Free-Running mode
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; //10-bit resolution
	ADC0.CTRLA |= ADC_FREERUN_bm; //Free-Running mode enabled
	ADC0.CTRLA |= ADC_ENABLE_bm; //Enable ADC
	ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc; //The bit
	//Enable Debug Mode
	ADC0.DBGCTRL |= ADC_DBGRUN_bm;
	//Window Comparator Mode
	ADC0.WINHT |= 10; //Set threshold
	ADC0.INTCTRL |= ADC_WCMP_bm; //Enable Interrupts for WCM
	ADC0.CTRLE |= ADC_WINCM0_bm; //Interrupt when RESULT < WINLT
	ADC0.COMMAND |= ADC_STCONV_bm; //Start Conversion
	
	
	TCA0.SINGLE.CTRLA=TCA_SINGLE_CLKSEL_DIV1024_gc;
	TCA0.SINGLE.PER = 254; //select the resolution
	TCA0.SINGLE.CMP0 = 127; //select the duty cycle
	//select Single_Slope_PWM
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	//enable interrupt Overflow
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	//enable interrupt COMP0
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_CMP0_bm;
	sei(); //begin accepting interrupt signals
	
while(1) //θερμοσίφωνας
{	
	if (PORTD.OUT |= PIN0_bm) //Σβηστό το LED0
	{
		if (PORTF.PIN5CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin5, break και πήγαινε σε επόμενη συσκευή
		{
			break;
		}
		else if (PORTF.PIN6CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc)//Αν πατηθεί το pin6 εκτέλεσε τον κώδικα
		{
			PORTD.OUTCLR = PIN0_bm; //Αναψε το LED0
			TCA0.SINGLE.CTRLA |=1;//Ενεργοποίηση timer
			TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; //Interrupt Enable (=0x10)
			sei();
			break;
		}
		else break;
	}
	else if(PORTD.OUTCLR = PIN0_bm) //Ανοιχτό το LED0
	{
		if (PORTF.PIN5CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc)//Αν πατηθεί το pin5, break και πήγαινε σε επόμενη συσκευή
		{
			break;
		}
		else if (PORTF.PIN6CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc)//Αν πατηθεί το pin6 εκτέλεσε τον κώδικα
		{
			PORTD.OUT |= PIN0_bm; //Σβήσε το LED0
			TCA0.SINGLE.CTRLA |=0;//Απενεργοποίηση timer
			break;
			
		}
		else break;
		
	}
	else break;
	
}
while(1)//ανεμιστηρας
{
	if (PORTD.OUT |= PIN1_bm)//Σβηστό το LED1
	{
		if (PORTF.PIN5CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin5, break και πήγαινε σε επόμενη συσκευή
		{
			break;
		}
		else if (PORTF.PIN6CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin6 εκτέλεσε τον κώδικα
		{
			TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; //Enable
			sei();
			break;
		}
		else break;
	}
	else if  (PORTD.OUTCLR = PIN1_bm)
	{
		if (PORTF.PIN5CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin5, break και πήγαινε σε επόμενη συσκευή
		{
			break;
		}
		else if (PORTF.PIN6CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin6 εκτέλεσε τον κώδικα
		{
			PORTD.OUT |= PIN1_bm;//Σβησε το LED1
			//disable PWM
			break;
		}
		else break;
		
	}
	else break;
	
}
while(1) //water leakage
{
	
	
	if (PORTF.PIN5CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin5, break και πήγαινε σε επόμενη συσκευή
	{
		break;
	}
	else if(PORTF.PIN6CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin6 εκτέλεσε τον κώδικα
	{
		if (PORTD.OUTCLR = PIN2_bm)//Ανοιχτό το LED2
		{
			PORTD.OUT |= PIN2_bm;//Σβησε το LED2
			break;
		}
		else if (PORTD.OUT |= PIN2_bm)//Σβηστό το LED2
		{
			break;
		}
		
	}
	else break;
	
}
while(1) //κλειδωμα σπιτιου
{
	if (PORTF.PIN5CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin5, break και πήγαινε σε επόμενη συσκευή
	{
		break;
	}
	else if(PORTF.PIN6CTRL |=PORT_PULLUPEN_bm |PORT_ISC_BOTHEDGES_gc) //Αν πατηθεί το pin6 εκτέλεσε τον κώδικα
	{
		if (PORTD.OUT |= PIN3_bm)//Σβηστό το LED3
		{
			TCA0.SINGLE.CTRLA |=0;
			//disable adc
			//disable pwm
			break;
		}
			
		else if (PORTD.OUTCLR = PIN3_bm)//Ανοιχτό το LED3
		{
			//κώδικας για switches
		
		}
		
	}
	else break;
	
}
	
	
	
}


