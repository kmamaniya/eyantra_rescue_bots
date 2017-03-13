#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char serial_data; //to store received data from UDR1
unsigned char serial_flag; //flag received data from UDR1

void clear_serial_data(){         //clear serial_data and serial_flag
	serial_data = 0;
	serial_flag = 0;
}

void zigbee_init()
{
	UCSR0B = 0x00; //disable while setting baud rate
	UCSR0A = 0x00;
	UCSR0C = 0x06;
	UBRR0L = 0x5F; //set baud rate lo
	UBRR0H = 0x00; //set baud rate hi
	UCSR0B = 0x98;
}

SIGNAL(USART0_RX_vect) 		// ISR for receive complete interrupt
{
	serial_data = UDR0; 				//making copy of data from UDR0 in 'data' variable
	serial_flag=1;                      // set serial_flag true
}
