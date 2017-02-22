/*
* Project Name: 	e-Yantra Project
* Author List: 		Karan Mamaniya
* Filename: 		ultrasonic.h
* Functions:		sonar_timer_init(), sonar_port_init(), sonar() , ultrasonic_init() , 
* Global Variables:	result,up,running,timerCounter
*
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define INSTR_PER_US 14.745           // instructions per microsecond
#define INSTR_PER_MS 14745.6          // instructions per millisecond
#define MAX_RESP_TIME_MS 20      			// maximum response time in milliseconds

volatile uint16_t result = 0;
volatile unsigned char up = 0;
volatile unsigned char running = 0;
volatile uint32_t timerCounter = 0;

/*
* Function Name:	sonar_timer_init
* Input:			NONE
* Output:			NONE
* Logic:			Configure Timer 0 in 8 bit counter mode
* Example call:		sonar_timer_init()
*/
void sonar_timer_init()
{
	// setup 8 bit timer & enable interrupts, timer increments to 255 and interrupts on overflow
	TCCR0B = (0<<CS02)|(0<<CS01)|(1<<CS00); // select internal clock
	TCNT0 = 0; // reset counter to zero
	TIMSK0 = 1<<TOIE0; // enable timer interrupt
}


/*
* Function Name:	sonar_timer_init
* Input:			NONE
* Output:			NONE
* Logic:			Configure Timer 0 in 8 bit counter mode
* Example call:		sonar_port_init()
*/
void sonar_port_init()
{
	DDRD = 0x01; // PB0 output - connected to Trig
	PORTD = 0x00; // clear
}


/*
* Function Name:	sonar_interrup_init
* Input:			NONE
* Output:			NONE
* Logic:			Configure INT1 to invoke ISR whenever a change of state
* Example call:		sonar_interrupt_init()
*/
void sonar_interrupt_init()
{
	EICRA |= (0 << ISC11) | (1 << ISC10); // to enable interrupt on change of state
	EIMSK |= (1 << INT1);      // Turns on INT1
}

/*
* Interrupt Name:	TIMER0 Overflow Interrupt
* Input:			Trigger
* Output:			NONE
* Logic:			Timer overflow interrupt is used each time when timer value passes 255
*/


SIGNAL(TIMER0_OVF_vect)
{
	if (up)
	{       					// voltage rise was detected previously
		timerCounter++; // count the number of overflows
						// dont wait too long for the sonar end response,
						//stop if time for measuring the distance exceeded limits
		uint32_t ticks = timerCounter * 256 + TCNT0;
		uint32_t max_ticks = (uint32_t)MAX_RESP_TIME_MS * INSTR_PER_MS; // this could be replaced with a value instead of multiplying
		if (ticks > max_ticks)
		{
			up = 0;          // stop counting timer value
			running = 0; 		 // ultrasound scan done
			result = -1; 		 // show that the measurement failed with a timeout
		}
	}
}



/*
* Interrupt Name:	External Interrupt 1
* Input:			Trigger
* Output:			NONE
* Logic:      Logic for measurement of time during different phases
							of Sonar usage
*/
SIGNAL(INT1_vect)
{
	if (running)
	{ 								//accept interrupts only after sonar was started
		if (up == 0) { 	// voltage rise, start time measurement
			up = 1;
			timerCounter = 0;
			TCNT0 = 0; 		// reset timer counter
		}
		else {					// voltage drop, stop time measurement
			up = 0;
			result = (timerCounter * 256 + TCNT0) / 773;
										// Uses time taken to calculate distance
			running = 0;
		}
	}
}



/*
* Function Name:	sonar
* Input:			NONE
* Output:			NONE
* Logic:			Start sonar with a trigger [ 10 microsecond pulse]
* Example Call:		sonar()
*/
void sonar() {
	//lcd_cursor(1,1);
	//lcd_string("Sonar start");
	PORTD &= 0xFE; // clear to zero for 1 microsecond
	_delay_us(1);
	PORTD |= 0x01; // set high for 10 microseconds
	running = 1;  // sonar started
	_delay_us(20);
	PORTD &= 0xFE; // clear
}


/*
* Function Name:	ultrasonic_init
* Input:			NONE
* Output:			NONE
* Logic:			Initializing ulrasonic sensor HC-SR04
* Example Call:		sonar()
*/
void ultrasonic_init()
{
	sonar_port_init();
	sonar_timer_init();
	sonar_interrupt_init();
}
