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
* Logic:			Function to initialize servo, motor, lcd, zigbee and ultrasonic sensor
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
	motor_velocity(248,240);		//set motor velocity to match right and left rotation
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
	for(i=0;i<4;i++){			//transmit end sequence to stop scan loop in control
		transmit_char(201);
	}
	while(1){
		clear_serial_data();
		lcd_clear();				//clear LCD
		lcd_home();
		lcd_string("1. Scan");		//Printing a menu of options
		lcd_cursor(2,1);
		lcd_string("2. Move");	
		while(serial_flag == 0){
			_delay_us(500);
		}
		switch(serial_data){
			case '1':	scanAndTransmit();		/* Calls function to scan and transmit data to the remote machine */										   
						break;
			case '2':	move();					//Calls the move function
						break;
			case '3':	return_home();			/* This function or command tells the bot to return to its initial position (home location) */					   
			default:	break;
		}
	}
	return 0;
}

