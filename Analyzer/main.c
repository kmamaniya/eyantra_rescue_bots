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
	servo_init();
	sei();
	motor_velocity(248,240);
}

int main(void)
{
	int i;
	init();
	for(i=0;i<4;i++){
		transmit_char(201);
	}
	lcd_clear();
	lcd_home();
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
			case '3':	return_home();
			default:	break;
		}
	}
}

