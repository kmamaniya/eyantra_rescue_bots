/*
*
* Project Name: 	e-Yantra Project
* Author List: 		Karan Mamaniya
* Filename: 		main.c
* Functions: 		init() , main() 
* Global Variables:	NONE
*/
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
/*
* Function Name:	init
* Input:			NONE
* Output:			NONE
* Logic:			Function to initialize motor, lcd, zigbee and ultrasonic sensor
* Example Call:		init()
*
*/
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

/*
* Function Name:	main
* Input:			NONE
* Output:			NONE
* Logic:			Function to initialize motor, lcd, zigbee and ultrasonic sensor
* Example Call:		main()
*
*/
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
		//Printing a menu of options
		lcd_string("1. Scan");		//Option 1 to scan room
		lcd_cursor(2,1);
		lcd_string("2. Move");		//Option 2 to move bot
		while(serial_flag == 0){
			_delay_us(1000);
		}
		switch(serial_data){
			case '1':	scanAndTransmit();		/* Calls function to scan and transmit data to 
												   the remote machine */
			break;
			case '2':	move();					//Calls the move function
			break;
			case '3':	return_home();			/* This function or command tells the bot to return
												   to its initial position (home location) */
			default:	break;
		}
	}
}

