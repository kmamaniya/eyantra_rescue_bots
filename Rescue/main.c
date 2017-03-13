/*
*
* Project Name: 	e-Yantra Project
* Author List: 		Karan Mamaniya
* Filename: 		main.c
* Functions: 		init() , main() 
* Global Variables:	linear[], angle[], moves
*/
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
	lcd_init();
	lcd_set_4bit();
	servo_init();
	zigbee_init();
	motor_init();
	sei();
	motor_velocity(250,254);
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
	init();
	release();
    while (1){
		lcd_clear();		//clear LCD
		lcd_home();
		lcd_string("1. MOVE 2. grab");	//Print menu options
		lcd_cursor(2,1);			
		lcd_string("3. retract  4. release");
		clear_serial_data();
		while(serial_flag!=1)
			_delay_ms(1000);
		switch(serial_data){
			case '1': move();break;				// Calls function to move bot
			case '2': grab();break;				// Calls function to grab human or debris
			case '3': return_home();break;		// Calls function to return to home location
			case '4': release();break;			/* Calls function to release hold of human or debris*/
			default: break;
		}
		clear_serial_data();
			
	}
}
