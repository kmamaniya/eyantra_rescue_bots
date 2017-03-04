#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char serial_data; //to store received data from UDR1

void start_sequence(){
	while(1){
		if(serial_data != 'a' || serial_data != 'A'){
			lcd_string("Program started");
			serial_data = 0;
			break;
		}
		_delay_us(500);
	}
}

void transmit_termination(uint16_t endSequence){
	UDR0 = endSequence;
	_delay_us(1000);
	UDR0 = endSequence;
	_delay_us(1000);
	UDR0 = 0;
	_delay_us(1000);
	UDR0 = endSequence;
	_delay_us(1000);
	UDR0 = 0;
	_delay_us(1000);
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

void zigbee_init(void)
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
	//lcd_wr_char(serial_data);
}

void clear_serial_data(){
	serial_data = 0;
}