#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */


volatile uint16_t distance_mm = 0;
volatile unsigned char up = 0;
volatile unsigned char running = 0;
volatile uint32_t timerCounter = 0;


/******* logic****************


// count the number of overflows
// don't wait too long for the sonar end response,
//stop if time for measuring the distance exceeded limits*/
SIGNAL(TIMER0_OVF_vect)
{
	if (up) // voltage rise was detected previously
	{       
		timerCounter++; 
		uint32_t ticks = timerCounter * 256 + TCNT0;
		uint32_t max_ticks = 1253325;				//max ticks set according to frequency
		if (ticks > max_ticks)						// timeout
		{
            up = 0;          
			running = 0;							// ultrasound scan done
			distance_mm = -1;						// show that measurement failed with a timeout (could return max distance here if needed)
		}
	}
}
// interrupt for INT1 pin, to detect high/low voltage changes
/**
        We assume, that high voltage rise comes before low drop and not vice versa -
        however this should be implemented more correctly using both interrupts INT0/INT1,
        (i.e. INT0 configured for high rise, and INT1 - for low rise, thus the code must be separated also)    
*/

SIGNAL(INT1_vect)
{
	if (running){				//accept interrupts only when sonar was started
		if (up == 0) {			// voltage rise, start time measurement
			up = 1;
			timerCounter = 0;
			TCNT0 = 0;			// reset timer counter			
		} 
		else {					// voltage drop, stop time measurement
			up = 0;
			distance_mm = (timerCounter * 256 + TCNT0) / 77.3;		// convert from time to distance(millimeters): d = [ time_s * 340m/s ]
			running = 0;		//stop sonar
		}
	}
}

void ultrasonic_distance() {
	PORTD &= 0xFE;			// clear to zero for 1 us
	_delay_us(1);
	PORTD |= 0x01;			// set high for 10us
	running = 1;			// ultrasonic launched
	_delay_us(20);
	PORTD &= 0xFE;			// clear 
}

void ultrasonic_init(){
	DDRD = 0x01;				// PB0 output - connected to Trig
	PORTD = 0x00;				// clear
	
	// turn on interrupts for INT1, connect Echo to INT1
	EICRA |= (0 << ISC11) | (1 << ISC10);		// enable interrupt on any(rising/dropping) edge
	EIMSK |= (1 << INT1);						// Turns on INT1
	
	// setup 8 bit timer & enable interrupts, timer increments to 255 and interrupts on overflow
	TCCR0B = (0<<CS02)|(0<<CS01)|(1<<CS00);		// select internal clock with no pre-scaling
	TCNT0 = 0;									// reset counter to zero
	TIMSK0 = 1<<TOIE0;							// enable timer interrupt
	sei();										// enable all(global) interrupts
}