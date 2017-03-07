/*
* Project Name: 	e-Yantra Project
* Author List: 		Karan Mamaniya
* Filename: 		zigbee.h
* Functions:		clear_serial_data(), start_sequence(), transmit_termination(uint8_t),transmit_array(uint16_t [],unsigned char [], unsigned char [],int )
* Global Variables:	serial_data
*
*/
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char serial_data; //to store received data from UDR1
unsigned char serial_flag; //flag received data from UDR1

/*
* Function Name:	clear_serial_data
* Input:			NONE
* Output:			NONE
* Logic:			Clears serial data
* Example call:		clear_serial_data()
*/
void clear_serial_data(){
	serial_data = 0;
	serial_flag = 0;
}

/*
* Function Name:	start_sequence
* Input:			NONE
* Output:			NONE
* Logic:			Starts program when char 'a' or 'A' is sent from computer
* Example call:		start_sequence()
*/
void start_sequence(){
	while(1){
		if(serial_data == 'a' || serial_data == 'A'){
			lcd_string("Program started");
			clear_serial_data();
			break;
		}
		_delay_us(500);
	}
}
/*
* Function Name:	transmit_termination
* Input:			uint8_t
* Output:			NONE
* Logic:			Sends endsequence to the python program
* Example call:		start_sequence()
*/
void transmit_termination(uint8_t endSequence){
	UDR0 = endSequence;
	_delay_us(1000);
}

/*
* Function Name:	transmit_array
* Input:			uint16_t, unsigned char, unsigned char, int
* Output:			NONE
* Logic:			Sends 16 bit arrays of r, theta and phi to the python program in one scan
* Example call:		transmit_array(r[],theta[],phil[],count)
*/
void transmit_array(uint16_t r[],unsigned char theta[], unsigned char phi[],int count){
	int i;
	for(i=0;i<count;i++){
		UDR0 = r[i]/256;
		_delay_us(1000);
		UDR0 = r[i]%256;
		_delay_us(1000);
		UDR0 = theta[i]/256;
		_delay_us(1000);
		UDR0 = theta[i]%256;
		_delay_us(1000);
		UDR0 = phi[i]/256;
		_delay_us(1000);
		UDR0 = phi[i]%256;
		_delay_us(1000);
	}
}

/*
* Function Name:	zigbee_init
* Input:			NONE
* Output:			NONE
* Logic:			Initializing Zigbee
* Example call:		zigbee_init()
*/
void zigbee_init()
{
	UCSR0B = 0x00; //disable while setting baud rate
	UCSR0A = 0x00;
	UCSR0C = 0x06;
	UBRR0L = 0x5F; //set baud rate lo
	UBRR0H = 0x00; //set baud rate hi
	UCSR0B = 0x98;
}
/*
* ISR Name:			SIGNAL
* Input:			NONE
* Output:			NONE
* Logic:			ISR for receive complete interrupt
* Example call:		SIGNAL(USART0_RX_vect)
*/
SIGNAL(USART0_RX_vect) 		// ISR for receive complete interrupt
{
	serial_data = UDR0; 				//making copy of data from UDR0 in 'data' variable
	serial_flag=1;
}

void transmit_char(char data){
	UDR0 = data;
	_delay_us(1000);
}