/*
* Project Name: 	e-Yantra Project
* Author List: 		Karan Mamaniya
* Filename: 		analyze.h
* Functions:		getNumber(char data), get_distance(), get_angle(), get_movement(), move(),
					scanAndTransmit()
* Global Variables:	linear[], angle[]
*
*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int linear[10];
int angle[10];

/*
* Function Name:	getNumber
* Input:			char
* Output:			int
* Logic:			Get individual 8 bit data from control center
* Example call:		getNumber(char d)
*/
int getNumber(char data){
	return data-'0';		//Obtain Decimal from ASCII input
}

/*
* Function Name:	get_distance
* Input:			NONE
* Output:			int
* Logic:			Get the distance to be travelled from the control center
* Example call:		get_distance()
*/
int get_distance(){
	lcd_clear();					//Clears lcd
	lcd_cursor(1,1);				//Sets lcd cursor at 1,1 before writing on lcd
	lcd_string("Enter distance");	//Displaying "Enter distance" on lcd screen
	int move_distance = 0;			//Initializing distances variable to zero
	clear_serial_data();			//Clears serial data
	while (serial_data != ',')		//Receive data until ','
	{
		if (serial_flag==1)			//append only when i/p is received
		{
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
* Output:			int
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
		linear[moves] = get_distance();
		angle[moves] = get_angle();
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
* Example call:		move()
*/
void move(){
	int moves,count=0;
	moves=get_movement();
	while(count<moves){
		right_degrees(angle[count]);
		forward();
		linear_distance_mm(linear[count]);
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
	servoTilt(20.46);
	_delay_ms(1000);
	lcd_clear();
	lcd_cursor(1,1);
	for(tilt_step=20.46;tilt_step<100;tilt_step+=1.86){
		servoTilt(tilt_step);
		_delay_ms(500);
		pan_step=0;
		lcd_print(2,1,tilt_step,3);
		for (count=0;pan_step<180;count++){
			servoPan(pan_step);
			sonar();
			_delay_ms(150);
			r[count]= distance_mm;
			theta[count] = (uint8_t)(pan_step);
			phi[count] = (uint8_t) (tilt_step);
			lcd_print(1,1,distance_mm,4);
			lcd_print(1,7,pan_step,3);
			distance_mm = 0;
			pan_step+=1.86;
		}
		servoPan(0);
		transmit_array(r,theta,phi,count);
		_delay_ms(500);
		lcd_clear();
	}
	transmit_termination(201);
	servoPan_free();
	servoTilt_free();
}