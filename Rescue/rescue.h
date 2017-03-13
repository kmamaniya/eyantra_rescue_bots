/*
*
* Project Name: 	e-Yantra Project
* Author List: 		Karan Mamanyia
* Filename: 		rescue.h
* Functions: 		getNumber, get_distance, get_angle, get_movement, move, grab, release,
					return_home
* Global Variables: linear_movement[], angle_movement[], moves
*
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int linear_movement[100];
int angle_movement[100];
int moves;

/*
* Function Name:	getNumber
* Input:			char
* Output:			int
* Logic:			Get individual 8 bit data from control center
* Example call:		getNumber('1')
*/
int getNumber(char data){
	return data-'0';		//Obtain Decimal from ASCII input
}

/*
* Function Name:	get_distance
* Input:			NONE
* Output:			returns distance in mm
* Logic:			Get the distance to be travelled from the control center
* Example call:		get_distance()
*/
int get_distance(){
	lcd_clear();					//Clears lcd
	lcd_cursor(1,1);				//Sets lcd cursor at 1,1 before writing on lcd
	lcd_string("Enter distance");	//Displaying "Enter distance" on lcd screen
	int move_distance = 0;			//Initializing distances variable to zero
	clear_serial_data();			//Clears serial data
	while (serial_data != ','){		//Receive data until ','
		if (serial_flag==1){			//append only when i/p is received
			move_distance *= 10;						//logic to obtain decimal
			move_distance += getNumber(serial_data);
			clear_serial_data();
		}
		lcd_cursor(2,1);								//display move_distance
		lcd_string("D = ");
		lcd_print(2,5,move_distance,4);
	}
	clear_serial_data();
	return move_distance;
}

/*
* Function Name:	get_angle
* Input:			NONE
* Output:			returns angle in degrees
* Logic:			Get the angle to be travelled from the control center
* Example call:		get_angle()
*/
int get_angle(){
	lcd_clear();										//Clears lcd
	int angle=0;										//Displaying "Enter angle" on lcd screen
	lcd_cursor(1,1);									//Sets lcd cursor at 1,1 before writing on lcd
	lcd_string("Enter angle");
	clear_serial_data();								//Clears serial data
	while (serial_data != ',' && serial_data != ';'){
		if (serial_flag==1){								//append only when i/p is received
			angle = getNumber(serial_data)*90;			//Store actual clockwise angle
			clear_serial_data();
		}
		lcd_cursor(2,1);								//display angle
		lcd_string("A = ");
		lcd_print(2,5,angle,3);
	}
	return angle;
}

/*
* Function Name:	get_movement
* Input:			NONE
* Output:			int
* Logic:			Get movement to be travelled by using the distance and angle
* Example call:		get_movement()
*/
int get_movement(){
	int moves=0;
	while(serial_data != ';'){
		linear_movement[moves] = get_distance();
		angle_movement[moves] = get_angle();
		moves++;
	}
	clear_serial_data();
	return moves;
}

/*
* Function Name:	move
* Input:			NONE
* Output:			NONE
* Logic:			Manoeuvre the bot from 3d analysis
					by deciding the distance and angle to be travelled
* Example call:		move()
*/
void move(){
	int count=0;
	moves=get_movement();
	while(count<moves){
		switch (angle_movement[count]){
			case 90: soft_right();angle_rotate(90);break;
			case 180: soft_left();angle_rotate(180);break;
			case 270: soft_left();angle_rotate(90);break;
			default:break;
		}
		forward();
		linear_distance_mm(linear_movement[count]);
		stop();
		_delay_ms(500);
		count++;
	}
}

/*
* Function Name:	grab
* Input:			NONE
* Output:			NONE
* Logic:			Function to grab object or human
* Example call:		grab()
*/
void grab(){
	//grabbing
	servoRight(70);
	servoLeft(110);
	_delay_ms(500);
	servoRight_free();
	servoLeft_free();
}

/*
* Function Name:	release
* Input:			NONE
* Output:			NONE
* Logic:			Function to release object or human
* Example call:		release()
*/
void release(){
	//releasing
	servoRight(0);
	servoLeft(180);
	_delay_ms(500);
	servoRight_free();
	servoLeft_free();
}

/*
* Function Name:	return_home
* Input:			NONE
* Output:			NONE
* Logic:			The bot uses this function to return
					retrace its path
* Example call:		return_home()
*/
void return_home(){
	int count = moves-1;
	while(count>=0){
		back();
		linear_distance_mm(linear_movement[count]);
		stop();
		switch (angle_movement[count]){
			case 270: soft_right2();angle_rotate(90);break;
			case 180: soft_right2();angle_rotate(180);break;
			case 90: soft_left2();angle_rotate(90);break;
			default:break;
		}
		_delay_ms(500);
		count--;
	}
}