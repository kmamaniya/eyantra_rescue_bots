/*
*
* Project Name: 	e-Yantra Project
* Author List: 		Karan Mamaniya
* Filename: 		servo.h
* Functions: 		servoPan_pin_config () , servoTilt_pin_config () , timer1_init () , servo_init () ,
*					servoPan (unsigned char) , servoTilt(unsigned char) , servoPan_free () , servoTilt_free()
* Global Variables:	NONE
*
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

	/*
	* Function Name:	servoPan_pin_config
	* Input:			NONE
	* Output:			NONE
	* Logic:			DDRB(DIRECTION REGISTER B) SET as output
						PORTB 5th bit IS SET TO LOGIC 1
						Configure PORTB 5 pin for servo motor 1 operation
	* Example Call:		servoPan_pin_config()
	*
	*/

void servoPan_pin_config ()	{
	DDRB  = DDRB | 0x80;  //making PORTB 7 pin output
	PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
}



	/*
	* Function Name:	servoTilt_pin_config
	* Input:			NONE
	* Output:			NONE
	* Logic:			DDRB(DIRECTION REGISTER B) SET as output
						PORTB 6th bit IS SET TO LOGIC 1
						Configure PORTB 6 pin for servo motor 2 operation
	* Example Call:		servoTilt_pin_config()
	*
	*/
void servoTilt_pin_config ()	{
	DDRB = DDRB | 0x40;		//making PORTB 6 pin output
	PORTB = PORTB | 0x40;	//setting PORTB 6 pin to logic 1
}

	/*
	* Function Name:	timer1_init
	* Input:			NONE
	* Output:			NONE
	* Logic:			Initializing timer 1 in 10 bit fast PWM Mode 
	* Example Call:		servoTilt_pin_config()
	*
	*/
void timer1_init ()	{
	TCCR1B = 0x00; //stop
	TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
	TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
	OCR1AH = 0x03;	//Output compare Register high value for servo 3
	OCR1AL = 0xFF;	//Output Compare Register low Value For servo 3
	OCR1BH = 0x03;	//Output compare Register high value for servo 2
	OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
	OCR1CH = 0x03;	//Output compare Register high value for servo 3
	OCR1CL = 0xFF;	//Output Compare Register low Value For servo 3
	ICR1H  = 0x03;	
	ICR1L  = 0xFF;
	TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
 					For Overriding normal port functionality to OCRnA outputs.
				  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
	TCCR1C = 0x00;
	TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}

	/*
	* Function Name:	servo_init
	* Input:			NONE
	* Output:			NONE
	* Logic:			Initialize the ports for the server motor operations 
	* Example Call:		servo_init()
	*
	*/
void servo_init ()	{
	servoPan_pin_config(); //Configure PORTB 5 pin for servo motor 3 operation
	servoTilt_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation
	timer1_init();
}
	
	/*
	* Function Name:	servoPan
	* Input:			NONE
	* Output:			NONE
	* Logic:			Function to rotate Servo 3 by a specified angle 
	* Example Call:		servoPan(unsigned char)
	*
	*/

void servoPan (unsigned char degrees)	{
	float PositionServo = 0;
	PositionServo = ((float)degrees / 1.86) + 35.0;
	OCR1CH = 0x00;
	OCR1CL = (unsigned char) PositionServo;
}



	
	/*
	* Function Name:	servoTilt
	* Input:			NONE
	* Output:			NONE
	* Logic:			Function to rotate Servo 2 by a specified angle
	* Example Call:		servoTilt(unsigned char)
	*
	*/

void servoTilt(unsigned char degrees)	{
	float PositionTiltServo = 0;
	PositionTiltServo = ((float)degrees / 1.86) + 35.0;
	OCR1BH = 0x00;
	OCR1BL = (unsigned char) PositionTiltServo;
}

	/*
	* Function Name:	servoPan_free
	* Input:			NONE
	* Output:			NONE
	* Logic:			servo_free functions unlocks the servo motors from the any angle and make them free 
						by giving 100% duty cycle at the PWM. This function can be used to reduce the power 
						consumption of the motor if it is holding load against the gravity.
						--->makes servo 1 free rotating

	* Example Call:		servoPan_free()
	*
	*/

void servoPan_free ()	{
	OCR1CH = 0x03;
	OCR1CL = 0xFF; //Servo 3 off
}

/*
* Function Name:	servoTilt_free
* Input:			NONE
* Output:			NONE
* Logic:			servo_free functions unlocks the servo motors from the any angle and make them free 
					by giving 100% duty cycle at the PWM. This function can be used to reduce the power 
					consumption of the motor if it is holding load against the gravity.
					--->makes servo 2 free rotating

* Example Call:		servoTilt_free()
*
*/

void servoTilt_free()	{ 
	OCR1BH = 0x03;
	OCR1BL = 0xFF; 	//Servo 2 off
}