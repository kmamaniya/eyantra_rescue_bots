#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"
#include "zigbee.h"
#include "motor.h"
#include "lcd.h"
#include "rescue.h"

int linear[100];
int angle[100];
int moves;

void init(){
	cli();
	lcd_init();
	lcd_set_4bit();
	servo_init();
	zigbee_init();
	motor_init();
	sei();
	motor_velocity(250,254);
}

int main(void)
{
	init();
	release();
	/* Replace with your application code */
	
    while (1) 
    {
		lcd_clear();
		lcd_home();
		lcd_string("1. MOVE 2. grab");
		lcd_cursor(2,1);
		lcd_string("3. retract  4. release");
		clear_serial_data();
		while(serial_flag!=1)
			_delay_ms(1000);
			switch(serial_data){
				case '1': move();break;
				case '2': grab();break;
				case '3': return_home();break;
				case '4': release();break;
				default: break;
			}
			clear_serial_data();
			
		}
}
