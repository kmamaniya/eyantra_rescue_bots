#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Configure PORTB 5 pin for servo motor 1 operation
void servoPan_pin_config ()	{
	DDRB = DDRB | 0x20;		//making PORTB 5 pin output
	PORTB = PORTB | 0x20;	//setting PORTB 5 pin to logic 1
}

//Configure PORTB 6 pin for servo motor 2 operation
void servoTilt_pin_config ()	{
	DDRB = DDRB | 0x40;		//making PORTB 6 pin output
	PORTB = PORTB | 0x40;	//setting PORTB 6 pin to logic 1
}

void timer1_init ()	{
	TCCR1B = 0x00; //stop
	TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
	TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
	OCR1AH = 0x03;	//Output compare Register high value for servo 1
	OCR1AL = 0xFF;	//Output Compare Register low Value For servo 1
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

//Initialize the ports
void servo_init ()	{
	servoPan_pin_config(); //Configure PORTB 5 pin for servo motor 1 operation
	servoTilt_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation
	timer1_init();
}


//Function to initialize all the peripherals


//Function to rotate Servo 1 by a specified angle in the multiples of 1.86 degrees
void servoPan (unsigned char degrees)	{
	float PositionPanServo = 0;
	PositionPanServo = ((float)degrees / 1.86) + 35.0;
	OCR1AH = 0x00;
	OCR1AL = (unsigned char) PositionPanServo;
}


//Function to rotate Servo 2 by a specified angle in the multiples of 1.86 degrees
void servoTilt(unsigned char degrees)	{
	float PositionTiltServo = 0;
	PositionTiltServo = ((float)degrees / 1.86) + 35.0;
	OCR1BH = 0x00;
	OCR1BL = (unsigned char) PositionTiltServo;
}

//servo_free functions unlocks the servo motors from the any angle 
//and make them free by giving 100% duty cycle at the PWM. This function can be used to 
//reduce the power consumption of the motor if it is holding load against the gravity.

void servoPan_free ()	{ //makes servo 1 free rotating
	OCR1AH = 0x03; 
	OCR1AL = 0xFF; //Servo 1 off
}

void servoTilt_free()	{ //makes servo 2 free rotating
	OCR1BH = 0x03;
	OCR1BL = 0xFF; //Servo 2 off
}