/*
 * Askisi3.c
 *
 * Created: 7/5/2021 6:22:41 μμ
 * Author : Marinos
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define EMPROS  1
#define ANTI -1
volatile int direction = EMPROS;	// Αρχίζουμε με την εμπρός κίνηση

#define MOVING 1					//Πορεία προς τα εμπρός
#define ARISTERA 2					//Αριστερή στροφή
#define DEXIA 3				//Δεξιά στροφή
#define TURN_AROUND 4				//Αναστροφή
volatile int state = MOVING;		//Η αρχική κατάσταση είναι κίνηση προς τα εμπρός

#define STROFES 8					//Μέγιστος αριθμός στροφών
volatile int turns = 0;				//αριθμός στροφών

#define TURN_LR_TIME 9
#define TURN_AROUND_TIME TURN_LR_TIME*2



void timer_init(int time) {
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CMP0 = time;
	TCA0.SINGLE.CTRLA |= 0x01;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
}


ISR(ADC0_WCOMP_vect) {
	ADC0.CTRLA &= ~(ADC_ENABLE_bm);
	int intflags = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intflags;
	
	if (direction == EMPROS) {
		state = ARISTERA;		//Στην κανονική πορεία η αριστερή στροφή γίνεται όχι με το button (δεξιά στροφή), αλλά με τον ADC0
		} else {
		state = DEXIA;		//Στην αντίθετη πορεία η δεξιά στροφή γίνεται με τον ADC0
	}
}

ISR(PORTF_PORT_vect) {
	
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;

	if (intflags & PIN6_bm) {		//Εφόσον έχει πατηθεί το button στο PIN6
		state = TURN_AROUND;
		return;
	}
	
	// Αν το interrupt προήλθε απο μη ύπαρξη τοίχου δεξιά
	if (intflags & PIN5_bm) {			//Αν έχει πατηθεί το button στο PIN6
		if (direction == EMPROS)
		state = DEXIA;			// Αν είμαστε σε κανονική πορεία, στρίψε δεξιά
		else
		state = ARISTERA;			// Αν είμαστε σε αντίστροφη πορεία, στρίψε αριστερά
	}
}



ISR(TCA0_CMP0_vect) {
	TCA0.SINGLE.CTRLA &= ~(0x01);
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;
	state = MOVING;
}

int main() {
	
	// PIN3-->αριστερά, PIN2-->ευθεία, PIN1-->δεξιά
	//Set PIN[1,2,3] σαν εξόδους
	PORTD.DIR |= (PIN1_bm|PIN2_bm|PIN3_bm);
	//Set PIN[1,2,3]--> OFF
	PORTD.OUT |= (PIN1_bm|PIN2_bm|PIN3_bm);
	
	// PIN5--> Όχι τοίχος για δεξιά στην ευθεία (ή αριστερά στην αναστροφή)
	// PIN6--> Για αναστροφή
	//Set PIN[5,6]-->Είσοδοι
	PORTF.DIR &= ~(PIN5_bm|PIN6_bm);
	//Ενεργοποίησε τον pull-up resistor and ενεργοποίησε τα interrupts με το button release
	PORTF.PIN5CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);  //Χρήση pullup resistor για το PINS 5
	PORTF.PIN6CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);	 //Χρήση pullup resistor για το PINS 6
	
	//Θέσε τον παλμό ρολογιού στο CPU_FREQUENCY/1024
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc;
	
	TCA0.SINGLE.CTRLB = 0;
	
	
	
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; // Select 10-bit resolution
	
	ADC0.CTRLA |= ADC_FREERUN_bm; // Free-Running mode
	
	ADC0.CTRLB |= ADC_SAMPNUM_ACC2_gc;// Use two samples per conversion
	
	ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc;// Connect the ADC0 to the ADC input pin 7, convert what is sensed on that pin
	
	ADC0.DBGCTRL |= ADC_DBGRUN_bm; // Debug Mode, ADC0 will continue to run in Break Debug mode when the CPU is halted
	// Window Comparator Mode
	// Threshold=10
	ADC0.WINLT |= 10;
	ADC0.INTCTRL |= ADC_WCMP_bm;// Enable Interrupts for WCM, based on the threshold above and in the way defined below	
	ADC0.CTRLE |= ADC_WINCM0_bm; // Interrupt when RESULT < WINLT, meaning when the value of ADC0.RES is below the threshold in ADC0.WINLT
	sei();

	while(1) {
		
		
		if (direction == EMPROS)
		if (turns >= STROFES)	//Σταματάει αν φτάσουμε στις 8 στροφές
		break;
		
		if (direction == ANTI)	//Στην αντίθετη πορεία σταματάει αν φτάσουμε στις 0 στροφές
		if (turns <= 0)
		break;
		
		if (state == MOVING){		//Αν είμαστε σε κίνηση
			// Enable the ADC
			ADC0.CTRLA |= ADC_ENABLE_bm;
			ADC0.COMMAND |= ADC_STCONV_bm;
			while(!(ADC0.INTFLAGS & ADC_RESRDY_bm));	//Για όση διάρκεια το bit ADC_RESRDY δεν είναι "1"
			PORTD.OUTCLR = PIN1_bm;						// PIN1 = 0-->για να ανάψει το led  ( κίνηση προς τα εμπρός )
			PORTD.OUTSET = PIN3_bm|PIN2_bm;				//PIN2 = 1 -->για να σβήσει το led  (  κίνηση προς τα αριστερά ) και PIN3=1--> ( κίνηση προς τα δεξιά )
			while (state == MOVING);
		}
		
		

		if (state == DEXIA) {					//Δεξιά στροφή.
			turns += direction;						//Αυξάνουμε τις στροφές κατά μία
			PORTD.OUTCLR = PIN3_bm;					//PIN3 = 0-->για να ανάψουμε το led ( κίνηση προς τα δεξιά )
			PORTD.OUTSET = PIN1_bm|PIN2_bm;			//Σβήνουμε τα υπόλοιπα leds
			timer_init(TURN_LR_TIME);
			PORTF.PIN5CTRL &= ~(PORT_ISC_FALLING_gc); // (Όχι εντολή για δεξιά στροφή)
			PORTF.PIN6CTRL &= ~(PORT_ISC_FALLING_gc); //(Όχι εντολή για αναστροφή)
			while (state == DEXIA);
			PORTF.PIN5CTRL |= PORT_ISC_FALLING_gc; // (Εντολή για δεξιά στροφή)
			PORTF.PIN6CTRL |= PORT_ISC_FALLING_gc; // (Εντολή για αναστροφή)
		}
		
		if (state == ARISTERA) {					//Αριστερή στροφή.
			turns += direction;						//Αυξάνουμε τις στροφές κατά μία
			PORTD.OUTCLR = PIN2_bm;					//PIN2 = 0--> για να ανάψουμε το led (κίνηση προς τα αριστερά)
			PORTD.OUTSET = PIN1_bm|PIN3_bm;			//PIN1, PIN3 σε "1" -->για να τα σβήσουμε τα άλλα δύο Leds
			timer_init(TURN_LR_TIME);				//Θέτουμε και ξεκινάμε τον timer  ότι τελείωσε η στροφή (διαρκεί 9 χρονικές στιγμές)
			//Επιθυμούμε κατά τη διάρκεια της στροφής να μην πάρουμε interrupt από τα κουμπιά στο PIN5 και PIN6
			PORTF.PIN5CTRL &= ~(PORT_ISC_FALLING_gc); 
			PORTF.PIN6CTRL &= ~(PORT_ISC_FALLING_gc); 
			while (state == ARISTERA);
			//Ενεργοποιούμε τα interrupts από τα κουμπιά που έχουμε συνδέσει στο PIN5 και στο PIN6
			PORTF.PIN5CTRL |= PORT_ISC_FALLING_gc;  
			PORTF.PIN6CTRL |= PORT_ISC_FALLING_gc;	
		}

		if (state == TURN_AROUND) {				//Αναστροφή
			turns += direction;					//Αυξάνουμε τις στροφές κατά μία στην αναστροφή  πορεία από τη κανονική
			if (direction == EMPROS)
			direction = ANTI;
			else
			direction = EMPROS;							// Ευθεία κατεύθυνση
			PORTD.OUTCLR = PIN3_bm|PIN2_bm|PIN1_bm;		//Ανάβουν όλα τα leds
			timer_init(TURN_AROUND_TIME);				//Θέτουμε και ξεκινάμε πάλι τον timer ότι τελείωσε η αναστροφή (διαρκεί το διπλάσιο δηλαδή 18 χρονικές στιγμές)
			PORTF.PIN5CTRL &= ~(PORT_ISC_FALLING_gc); //
			PORTF.PIN6CTRL &= ~(PORT_ISC_FALLING_gc);
			while (state == TURN_AROUND);				// Μέχρι το πέρας της αναστροφής
			PORTF.PIN5CTRL |= PORT_ISC_FALLING_gc;
			PORTF.PIN6CTRL |= PORT_ISC_FALLING_gc;
		}
	}
}

