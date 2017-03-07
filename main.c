#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "servo.h"
#include "zigbee.h"
#include "lcd.h"
#include "ultrasonic.h"
#include "analyze.h"
#include "motor.h"

void init(){
	cli();
	motor_init();
	lcd_init();
	lcd_set_4bit();
	zigbee_init();
	ultrasonic_init();
	sei();
}

int main(void)
{
	int i;
	init();
	lcd_string("Initialing..");
	for (i=0;i<3;i++)
	{
		lcd_print(1,13,3-i,1);
		_delay_ms(1000);
	}
	lcd_clear();
	lcd_home();
	lcd_string("\' to start...");
	clear_serial_data();
	while(serial_data != '\''){
		_delay_us(1000);
	}
	while(1){
		clear_serial_data();
		lcd_clear();
		lcd_home();
		lcd_string("1. Scan");
		lcd_cursor(2,1);
		lcd_string("2. Move");
		while(serial_flag == 0){
			_delay_us(1000);
		}
		switch(serial_data){
			case '1':	scanAndTransmit();
						break;
			case '2':	move();
			break;
			default:	break;
		}
	}
}

