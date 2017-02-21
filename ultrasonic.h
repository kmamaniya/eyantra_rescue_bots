#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#define INSTR_PER_US 14.745                   // instructions per microsecond (depends on MCU clock, 12MHz current)
#define INSTR_PER_MS 14745.6                // instructions per millisecond (depends on MCU clock, 12MHz current)
#define MAX_RESP_TIME_MS 20      // timeout - max time to wait for low voltage drop (higher value increases measuring distance at the price of slower sampling)

volatile long result = 0;
volatile unsigned char up = 0;
volatile unsigned char running = 0;
volatile uint32_t timerCounter = 0;

SIGNAL(TIMER0_OVF_vect)
{
	if (up) 
	{       // voltage rise was detected previously
		timerCounter++; // count the number of overflows
						// dont wait too long for the sonar end response, 
						//stop if time for measuring the distance exceeded limits
		uint32_t ticks = timerCounter * 256 + TCNT0;
		uint32_t max_ticks = (uint32_t)MAX_RESP_TIME_MS * INSTR_PER_MS; // this could be replaced with a value instead of multiplying
		if (ticks > max_ticks) 
		{
                        // timeout
			up = 0;          // stop counting timer value
			running = 0; // ultrasound scan done
			result = -1; // show that measurement failed with a timeout (could return max distance here if needed)
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
	if (running) 
	{ //accept interrupts only when sonar was started
		if (up == 0) { // voltage rise, start time measurement
			up = 1;
			timerCounter = 0;
			TCNT0 = 0; // reset timer counter			
		} 
		else {	// voltage drop, stop time measurement
			up = 0;
                        // convert from time to distance(millimeters): d = [ time_s * 340m/s ] / 2 = time_us/58
			result = (timerCounter * 256 + TCNT0) / 773;
			running = 0;
		}
	}
}

void sonar() {
	lcd_cursor(1,1);
	lcd_string("Sonar start");
	PORTD &= 0xFE; // clear to zero for 1 us
	_delay_us(1);
	PORTD |= 0x01; // set high for 10us
	running = 1;  // sonar launched
	_delay_us(20);
	PORTD &= 0xFE; // clear 
}

void ultrasonic_init()
{
	DDRD = 0x01; // PB0 output - connected to Trig
	PORTD = 0x00; // clear
	// turn on interrupts for INT1, connect Echo to INT1
	EICRA |= (0 << ISC11) | (1 << ISC10); // enable interrupt on any(rising/droping) edge
	EIMSK |= (1 << INT1);      // Turns on INT1
	// setup 8 bit timer & enable interrupts, timer increments to 255 and interrupts on overflow
	TCCR0B = (0<<CS02)|(0<<CS01)|(1<<CS00); // select internal clock with no prescaling
	TCNT0 = 0; // reset counter to zero
	TIMSK0 = 1<<TOIE0; // enable timer interrupt
	sei(); // enable all(global) interrupts
}