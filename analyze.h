#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int linear[10];
int angle[10];

int getNumber(char data){
	return data-'0';
}

int get_distance(){
	lcd_clear();
	lcd_cursor(1,1);
	lcd_string("Enter distance");
	int distance = 0;
	clear_serial_data();
	while (serial_data != ',')
	{
		if (serial_flag==1)
		{
			distance *= 10;
			distance += getNumber(serial_data);
			clear_serial_data();
		}
		lcd_cursor(2,1);
		lcd_string("D = ");
		lcd_print(2,5,distance,4);
	}
	clear_serial_data();
	return distance;
}

int get_angle(){
	lcd_clear();
	int angle=0;
	lcd_cursor(1,1);
	lcd_string("Enter angle");
	clear_serial_data();
	while (serial_data != ',' && serial_data != ';'){
		if (serial_flag==1)
		{
			angle = getNumber(serial_data)*90;
			clear_serial_data();
		}
		lcd_cursor(2,1);
		lcd_string("A = ");
		lcd_print(2,5,angle,3);
	}
	return angle;
}


int get_movement(){
	int moves=0;
	while(serial_data != ';'){
		angle[moves] = get_angle();
		linear[moves] = get_distance();
		moves++;
	}
	clear_serial_data();
	return moves;
}

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