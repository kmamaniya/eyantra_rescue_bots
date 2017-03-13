/*

* Project Name: 	e-Yantra Project (SSRR)
* Author List: 		Karan Mamaniya
* Filename: 		analyze.h
* Functions: 		getNumber(char), get_distance(), get_angle(), get_movement(), move(),
					scanAndTransmit(), return_home()
* Global Variables:	linear_movements[ ], angle_movements[ ], moves
*
*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int linear_movements[10];
int angle_movements[10];
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
	int distance = 0;				//Initializing distances variable to zero
	clear_serial_data();			//Clears serial data
	while (serial_data != ',')		//Receive data until ','
	{
		if (serial_flag==1)			//append only when i/p is received
		{
			distance *= 10;								//logic to obtain decimal
			distance += getNumber(serial_data);
			clear_serial_data();
		}
		lcd_cursor(2,1);								//display move_distance
		lcd_string("D = ");
		lcd_print(2,5,distance,4);
	}
	clear_serial_data();
	return distance;
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
		if (serial_flag==1)								//append only when i/p is received
		{
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
		linear_movements[moves] = get_distance();
		angle_movements[moves] = get_angle();
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
		switch (angle_movements[count]){
			case 90: soft_right();angle_rotate(90);break;
			case 180: soft_left();angle_rotate(180);break;
			case 270: soft_left();angle_rotate(90);break;
			default:break;
		}
		forward();
		linear_distance_mm(linear_movements[count]);
		stop();
		_delay_ms(500);
		count++;
	}
}
/*
* Function Name:	scanAndTransmit
* Input:			NONE
* Output:			NONE
* Logic:			Scans and Transmits data
* Example call:		scanAndTransmit()
*/
void scanAndTransmit(){
	float pan_step,tilt_step;
	int count=0;
	uint16_t r[200];
	unsigned char theta[200],phi[200];
	servoPan(0);
	_delay_ms(1000);
	servoTilt(44.46);
	_delay_ms(1000);
	lcd_clear();
	lcd_cursor(1,1);
	for(tilt_step=44.64;tilt_step<70;tilt_step+=1.86){
		servoTilt(tilt_step);
		_delay_ms(500);
		pan_step=0;
		lcd_print(2,1,tilt_step,3);
		for (count=0;pan_step<180;count++){
			servoPan(pan_step);
			ultrasonic_distance();
			lcd_print(2,5,up,1);
			lcd_print(2,7,running,1);
			_delay_ms(150);
			r[count]= distance_mm;
			theta[count] = (uint8_t)(pan_step);
			phi[count] = (uint8_t) (tilt_step);
			lcd_print(1,1,distance_mm,4);
			lcd_print(1,7,pan_step,3);
			pan_step+=1.86;
			distance_mm = 0;
		}
		servoPan(0);
		transmit_array(r,theta,phi,count);
		_delay_ms(500);
		lcd_clear();
	}
	for (count = 0; count < 4; count++)
	{
		transmit_char(201);
	}
	servoPan_free();
	servoTilt_free();
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
	while(1){
		moves=get_movement();
		int count = moves-1;
		back();
		linear_distance_mm(linear_movements[count]);
		stop();
		switch (angle_movements[count]){
			case 270: soft_right2();angle_rotate(90);break;
			case 180: soft_right2();angle_rotate(180);break;
			case 90: soft_left2();angle_rotate(90);break;
			default:break;
		}
		transmit_char('$');
		_delay_ms(500);
	}
}
