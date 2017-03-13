#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char serial_data; //to store received data from UDR1
unsigned char serial_flag; //flag received data from UDR1

void clear_serial_data(){
	serial_data = 0;			//clear serial data
	serial_flag = 0;			//clear serial flag
}

void transmit_char(uint8_t endSequence){
	UDR0 = endSequence;
	_delay_us(1000);
}

void transmit_array(uint16_t r[],unsigned char theta[], unsigned char phi[],int count){
	int i;
	for(i=0;i<count;i++){
		UDR0 = r[i]/256;
		_delay_us(1000);
		UDR0 = r[i]%256;
		_delay_us(1000);
		UDR0 = theta[i];
		_delay_us(1000);
		UDR0 = phi[i];
		_delay_us(1000);
	}
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
	serial_flag=1;
	if(serial_data == '/'){
		clear_serial_data();
	}
}