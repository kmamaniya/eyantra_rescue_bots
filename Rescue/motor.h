#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
volatile unsigned int Degrees; //to accept angle in degrees for turning
int left_dir = -1, right_dir = 1;
int checkBit=0;

void motion_pin_config (void){
	DDRA = DDRA | 0x0F;
	PORTA = PORTA & 0xF0;
	DDRL = DDRL | 0x18;
	PORTL = PORTL | 0x18;
}

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

void motion_set (unsigned char Direction){
	unsigned char PortARestore = 0;
	Direction &= 0x0F;
	PortARestore = PORTA;
	PortARestore &= 0xF0;
	PortARestore |= Direction;
	PORTA = PortARestore;
}

void forward (){
	motion_set(0x06);
}

void back (){
	motion_set(0x09);
}

void left (void){
	motion_set(0x05);
}

void right (void){
	motion_set(0x0A);
}

void soft_left (void){
	motion_set(0x04);
}

void soft_right2(){
	motion_set(0x08);
}

void stop(){
	motion_set(0x00);
}

void soft_right (void){ //Left wheel forward, Right wheel is stationary
	motion_set(0x02);
}

void soft_left2(){
	motion_set(0x01);
}

void motor_velocity (unsigned char left_motor, unsigned char right_motor){
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

void left_encoder_pin_config (void){
	DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

void right_encoder_pin_config (void){
	DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

void motor_port_init(){
	motion_pin_config(); //robot motion pins config
	left_encoder_pin_config(); //left encoder pin config
	right_encoder_pin_config(); //right encoder pin config
}

void left_position_encoder_interrupt_init (void){ //Interrupt 4 enable

	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
	EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
	sei();   // Enables the global interrupt
}

void right_position_encoder_interrupt_init (void){ //Interrupt 5 enable
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
	EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
	sei();   // Enables the global interrupt
}

ISR(INT5_vect){
	ShaftCountRight++;  //increment right shaft position count
}

ISR(INT4_vect){
	ShaftCountLeft++;  //increment left shaft position count
}

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

void motor_init(){
	motor_port_init();
	timer5_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();
}