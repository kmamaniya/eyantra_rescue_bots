#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
#include "servo.h"
#include "ultrasonic.h"
#include "zigbee.h"

void scanAndTransmit(){
	//motors stop code
	float pan_step,tilt_step;
	int count=0;
	uint16_t r[100];
	unsigned char theta[100],phi[100];
	
	servoPan(0);
	_delay_ms(1000);
	servoTilt(20.46);
	_delay_ms(1000);
	
	for(tilt_step=20.46;tilt_step<100;tilt_step+=1.86){
		servoTilt(tilt_step);
		_delay_ms(500);
		pan_step=0;
		for (count=0;pan_step<180;count++){
			servoPan(pan_step);
			_delay_ms(50);
			sonar();
			r[count]= distance_mm;
			theta[count] = (unsigned char)pan_step;
			phi[count] = (unsigned char)tilt_step;
			lcd_print(2,1,distance_mm,4);
			pan_step+=2*1.86;
		}
		servoPan(0);
		transmit_array(r,theta,phi,count);
		transmit_termination(200);
		_delay_ms(500);
	}
	transmit_termination(201);
	servoPan_free();
	servoTilt_free();
}

void init(){
	cli();
	lcd_init();
	lcd_set_4bit();
	lcd_string("Initializing....");
	servo_init();
	ultrasonic_init();
	zigbee_init();
	sei();
}

int main(void)
{
	init();
	lcd_home();
	lcd_string("Send 'a'");
	lcd_home();
	lcd_string("Program start");
	while (1)
	{
		scanAndTransmit();
		while (1);
	}
	return 0;
}