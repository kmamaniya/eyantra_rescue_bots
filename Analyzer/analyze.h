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

int linear_movements[10];             //to store linear movements
int angle_movements[10];              //to store angular movements
int moves;                            //to store number of moves

/*
* Function Name:	getNumber
* Input:			char
* Output:			int
* Logic:			decode decimal from ASCII data
* Example call:		decimalDigit = getNumber('1')
*/
int getNumber(char data){
	return data-'0';		//Obtain Decimal from ASCII input
}

/*
* Function Name:	get_distance
* Input:			NONE
* Output:			returns distance in mm
* Logic:			Get the distance to be travelled from the control center
* Example call:		linear_movement[i] = get_distance()
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
		lcd_cursor(2,1);								//display move_distance received
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
* Example call:		angle_movement[i] = get_angle()
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
* Output:			returns total number of moves and populates angle_movement[] & linear_movement[]
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
* Output:			robot navigation
* Logic:			Manoeuvre the bot from 3d analysis
					by deciding the distance and angle to be travelled
* Example call:		move()
*/
void move(){
	int count=0;
	moves=get_movement();       
	while(count<moves){             
		switch (angle_movements[count]){               //first execute turn instruction
			case 90: soft_right();angle_rotate(90);break;
			case 180: soft_left();angle_rotate(180);break;
			case 270: soft_left();angle_rotate(90);break;
			default:break;
		}
		forward();                                       //execute linear movement
		linear_distance_mm(linear_movements[count]);
		stop();
		_delay_ms(500);
		count++;
	}
}
/*
* Function Name:	scanAndTransmit
* Input:			NONE
* Output:			serial data output
* Logic:			Scans and Transmits data
* Example call:		scanAndTransmit()
*/
void scanAndTransmit(){
	float pan_step,tilt_step;           //step angles
	int count=0;
	uint16_t r[200];                    //radius in spherical co-ordinates system
	unsigned char theta[200],phi[200];  // theta and phi in spherical co-ordinate system
	servoPan(0);                        //intiatial servo positions        
	_delay_ms(1000);
	servoTilt(44.46);                   //intiatial servo positions        
	_delay_ms(1000);
	lcd_clear();
	lcd_cursor(1,1);
	for(tilt_step=44.64;tilt_step<70;tilt_step+=1.86){           //scan from tilt angle 44.64 degrees 
		servoTilt(tilt_step);                                    //to 70 degrees
		_delay_ms(500);                                          //and pan angle 0 degrees to 180 degrees
		pan_step=0;
		lcd_print(2,1,tilt_step,3);
		for (count=0;pan_step<180;count++){
			servoPan(pan_step);
			ultrasonic_distance();                               //start ultrasonic sensor
			_delay_ms(150);                                      
			r[count]= distance_mm;                               //store r, phi and theta values
			theta[count] = (uint8_t)(pan_step);
			phi[count] = (uint8_t) (tilt_step);
			lcd_print(1,1,distance_mm,4);
			lcd_print(1,7,pan_step,3);
			pan_step+=1.86;
			distance_mm = 0;                                     //reset distance_mm
		}
		servoPan(0);
		transmit_array(r,theta,phi,count);                       //serially transmit all the obtained data in single sweep
		_delay_ms(500);
		lcd_clear();
	}
	for (count = 0; count < 4; count++)                          //End sequence r = 201,201
	{                                                            //phi = 201
		transmit_char(201);                                      //theta = 201 to imply end of transmission
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
	while(1){                                                    //bot remains in while once this command is executed
		moves=get_movement();                                    //get number or move instructions
		int count = moves-1;                                     
		back();                                                  // reverse linear motion
		linear_distance_mm(linear_movements[count]);             // reverse angular motion
		stop();
		switch (angle_movements[count]){                         // reverse angular motion
			case 270: soft_right2();angle_rotate(90);break;
			case 180: soft_right2();angle_rotate(180);break;
			case 90: soft_left2();angle_rotate(90);break;
			default:break;
		}
		transmit_char('$');                                      //send acknowledgemnt character '$'
		_delay_ms(500);
	}
}
