/*
*
* Project Name: 	e-Yantra Project
* Author List: 		Sen Rajan Mathew, Karan Mamanyia
* Filename: 		motor.h
* Functions: 		motion_pin_config, timer5_init, motion_set, forward, back, left, right,
					soft_left, soft_right, soft_left2, soft_right2, stop, motor_velocity,
					left_encoder_pin_config, right_encoder_pin_config, motor_port_init,
					left_position_encoder_interrupt_init, right_position_encoder_interrupt_init,
					linear_distance_mm, angle_rotate, motor_init
* Global Variables: ShaftCountLeft, ShaftCountRight, Degrees, left_dir, right_dir, checkBit
*
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
volatile unsigned int Degrees; //to accept angle in degrees for turning
int left_dir = -1, right_dir = 1;
int checkBit=0;

/*
* Function Name:	motion_pin_config
* Input:			NONE
* Output:			NONE
* Logic:			Motor pins are initialized
* Example Call:		motion_pin_config()
*
*/
void motion_pin_config (void){
	DDRA = DDRA | 0x0F;
	PORTA = PORTA & 0xF0;
	DDRL = DDRL | 0x18;
	PORTL = PORTL | 0x18;
}

/*
* Function Name:	timer5_init
* Input:			NONE
* Output:			NONE
* Logic:			Timer 5 is set for PWM generation
* Example Call:		timer5_init()
*
*/
void timer5_init(){
	TCCR5B = 0x00;
	TCNT5H = 0xFF;
	TCNT5L = 0x01;
	OCR5AH = 0x00;
	OCR5AL = 0xFF;
	OCR5BH = 0x00;
	OCR5BL = 0xFF;
	OCR5CH = 0x00;
	OCR5CL = 0xFF;
	TCCR5A = 0xA9;
	TCCR5B = 0x0B;
}

/*
* Function Name:	motion_set
* Input:			char direction
* Output:			NONE
* Logic:			Set motor motion
* Example Call:		motion_set(char d)
*
*/
void motion_set (unsigned char Direction){
	unsigned char PortARestore = 0;
	Direction &= 0x0F;
	PortARestore = PORTA;
	PortARestore &= 0xF0;
	PortARestore |= Direction;
	PORTA = PortARestore;
}

/*
* Function Name:	forward
* Input:			NONE
* Output:			NONE
* Logic:			To move bot forward
* Example Call:		forward()
*
*/
void forward (){
	motion_set(0x06);
}

/*
* Function Name:	back
* Input:			NONE
* Output:			NONE
* Logic:			To move bot backward
* Example Call:		back()
*
*/
void back (void)
{
	motion_set(0x09);
}

/*
* Function Name:	left
* Input:			NONE
* Output:			NONE
* Logic:			To move bot to the left
* Example Call:		left()
*
*/
void left (void)
{
	motion_set(0x05);
}

/*
* Function Name:	right
* Input:			NONE
* Output:			NONE
* Logic:			To move bot to the right
* Example Call:		left()
*
*/
void right (void)
{
	motion_set(0x0A);
}

/*
* Function Name:	soft_left
* Input:			NONE
* Output:			NONE
* Logic:			To move bot to the left using right wheel forward, left wheel is stationary
* Example Call:		soft_left()
*
*/
void soft_left (void){
	motion_set(0x04);
}

/*
* Function Name:	stop
* Input:			NONE
* Output:			NONE
* Logic:			To stop bot
* Example Call:		stop()
*
*/
void stop(){
	motion_set(0x00);
}

/*
* Function Name:	soft_right
* Input:			NONE
* Output:			NONE
* Logic:			To move bot to the right using left wheel forward, right wheel is stationary
* Example Call:		soft_right()
*
*/
void soft_right (void){ //Left wheel forward, Right wheel is stationary
	motion_set(0x02);
}

void soft_left2(){
	motion_set(0x01);
}

void soft_right2(){
	motion_set(0x08);
}

/*
* Function Name:	motor_velocity
* Input:			left_motor speed, right_motor speed
* Output:			NONE
* Logic:			Set speed for both motors/wheels
* Example Call:		motor_velocity(char l, char r)
*
*/
void motor_velocity (unsigned char left_motor, unsigned char right_motor){
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

/*
* Function Name:	left_encoder_pin_config
* Input:			NONE
* Output:			NONE
* Logic:			Set the direction of the PORTE 4 pin as input and enable internal pull-up for PORTE 4 pin
* Example Call:		left_encoder_pin_config()
*
*/
void left_encoder_pin_config (void){
	DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

/*
* Function Name:	right_encoder_pin_config
* Input:			NONE
* Output:			NONE
* Logic:			Function to configure INT5 (PORTE 5) pin as input for the right position encoder
* Example Call:		right_encoder_pin_config()
*
*/
void right_encoder_pin_config (void){
	DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

/*
* Function Name:	motor_port_init
* Input:			NONE
* Output:			NONE
* Logic:			Function to initialize ports
* Example Call:		motor_port_init()
*
*/
void motor_port_init(){
	motion_pin_config(); //robot motion pins config
	left_encoder_pin_config(); //left encoder pin config
	right_encoder_pin_config(); //right encoder pin config
}

/*
* Function Name:	left_position_encoder_interrupt_init
* Input:			NONE
* Output:			NONE
* Logic:			Interrupt initialization for left position encoder.
* Example Call:		left_position_encoder_interrupt_init()
*
*/
void left_position_encoder_interrupt_init (void){ //Interrupt 4 enable

	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
	EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
	sei();   // Enables the global interrupt
}

/*
* Function Name:	right_position_encoder_interrupt_init
* Input:			NONE
* Output:			NONE
* Logic:			Interrupt initialization for right position encoder.
* Example Call:		right_position_encoder_interrupt_init()
*
*/
void right_position_encoder_interrupt_init (void){ //Interrupt 5 enable
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
	EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
	sei();   // Enables the global interrupt
}

/*
* ISR Name:	INT5_vect
* Input:			NONE
* Output:			NONE
* Logic:			ISR for right position encoder
*
*/
ISR(INT5_vect){
	ShaftCountRight++;  //increment right shaft position count
}

/*
* ISR Name:	INT4_vect
* Input:			NONE
* Output:			NONE
* Logic:			ISR for left position encoder
*
*/
ISR(INT4_vect){
	ShaftCountLeft++;  //increment left shaft position count
}


/*
* Function Name:	linear_distance_mm
* Input:			int DistanceInMM
* Output:			NONE
* Logic:			Bot moves till the desired distance is obtained
* Example Call:		linear_distance_mm()
*
*/
void linear_distance_mm(unsigned int DistanceInMM){
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;
	ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
	ShaftCountRight = 0;
	while(1){
		if(ShaftCountRight > ReqdShaftCountInt){
			break;
		}
	}
	stop();
}

/*
* Function Name:	angle_rotate
* Input:			int Degrees
* Output:			NONE
* Logic:			Function used for turning robot by specified degrees 
* Example Call:		angle_rotate(int d)
*
*/
void angle_rotate(unsigned int Degrees){
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;
	ReqdShaftCount = (float) Degrees/ 2.045; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	ShaftCountRight = 0;
	ShaftCountLeft = 0;
	while (1){
		if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))//| Center_white_line > 100)
		break;
	}
	stop(); //Stop robot
}

/*
* Function Name:	motor_init
* Input:			NONE
* Output:			NONE
* Logic:			Initialize motor, timer5 and position encoders
* Example Call:		motor_init()
*
*/
void motor_init(){
	motor_port_init();
	timer5_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();
}