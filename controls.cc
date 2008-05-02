//======================================================================================
/** \file  controls.cc
 *  Class which implements basic controls algorithms for the ME405 board
 *
 *  Revisions:
 *    \li  05-01-08  Created files
 *    \li  05-01-08  Avoiding splitting into gear_controls class and controls class
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

// Includes libraries
#include <stdlib.h>  
#include <avr/io.h>
#include <avr/interrupt.h> 

// Including header files
#include "rs232.h"      
#include "motor_driver.h"
#include "controls.h"


/** These defines make it easier for us to manipulate the bits of our registers, by
 * creating two new commands - cbi for clear bit i and sbi for set bit i
 */

#define BV(bit) (1<<(bit)) // Byte Value => sets bit'th bit to 1
#define cbi(reg, bit) reg &= ~(BV(bit)) // Clears the corresponding bit in register reg
#define sbi(reg, bit) reg |= (BV(bit))  // Sets the corresponding bit in register reg

/** ISR's for updating the encoder position
 */
ISR(INT4){
	if( (PORTE & 0b00010000) == 0b00010000 ){
		if(encoder_pin_A == false && encoder_pin_B == true){
			motor_position == encoder_max_value ? motor_position = 0 : motor_position += 1;
			gear_position == encoder_gear_max_value ? gear_position = 0 : gear_position += 1;
		}
		else if(encoder_pin_A == false && encoder_pin_B == false){
			motor_position == 0 ? motor_position = encoder_max_value : motor_position -= 1;
			gear_position == 0 ? gear_position = encoder_gear_max_value : gear_position -= 1;
		}
		else{
			error_count += 1;
		}
		encoder_pin_A = true;
	}
	else{
		if(encoder_pin_A == true && encoder_pin_B == false){
			motor_position == encoder_max_value ? motor_position = 0 : motor_position += 1;
			gear_position == encoder_gear_max_value ? gear_position = 0 : gear_position += 1;
		}
		else if(encoder_pin_A == true && encoder_pin_B == true){
			motor_position == 0 ? motor_position = encoder_max_value : motor_position -= 1;
			gear_position == 0 ? gear_position = encoder_gear_max_value : gear_position -= 1;
		}
		else{
			error_count += 1;
		}
		encoder_pin_A = false;
	}
}

ISR(INT5){
	if( (PORTE & 0b00100000) == 0b00100000 ){
		if(encoder_pin_A == false && encoder_pin_B == false){
			motor_position == encoder_max_value ? motor_position = 0 : motor_position += 1;
			gear_position == encoder_gear_max_value ? gear_position = 0 : gear_position += 1;
		}
		else if(encoder_pin_A == true && encoder_pin_B == false){
			motor_position == 0 ? motor_position = encoder_max_value : motor_position -= 1;
			gear_position == 0 ? gear_position = encoder_gear_max_value : gear_position -= 1;
		}
		else{
			error_count += 1;
		}
		encoder_pin_B = true;
	}
	else{
		if(encoder_pin_A == true && encoder_pin_B == true){
			motor_position == encoder_max_value ? motor_position = 0 : motor_position += 1;
			gear_position == encoder_gear_max_value ? gear_position = 0 : gear_position += 1;
		}
		else if(encoder_pin_A == false && encoder_pin_B == true){
			motor_position == 0 ? motor_position = encoder_max_value : motor_position -= 1;
			gear_position == 0 ? gear_position = encoder_gear_max_value : gear_position -= 1;
		}
		else{
			error_count += 1;
		}
		encoder_pin_B = false;
	}
}

//-------------------------------------------------------------------------------------
/** This constructor initializes the data members of the controls class, and also
 *  calls the motor driver constructor to set up the motor driver variables
 */

controls::controls (base_text_serial* p_serial_port) : motor_driver(p_serial_port)
{
	// Setup the interrupt pins as input pins
	cbi(DDRE, DDE5);
	cbi(DDRE, DDE4);

	// Zero error_count
	error_count = 0;

	// Enable interrupts
	sei();
	
	// Initialize constants
	motor_position = 0;
	gear_position = 0;
	ki = 0;
	kp = 0;
	kd = 0;

	// Number of encoder ticks per revolution of motor
	encoder_max_value = 2015;
	// Gear ratio of this project
	gear_ratio = 16;
	// Number of encoder ticks to equal a full gear revolution
	encoder_gear_max_value = (long)((encoder_max_value + 1) * gear_ratio) - 1;
}

//-------------------------------------------------------------------------------------
/** This method sets the current position as the new reference state for angle 
 *  measurements passed to the controller
 */

void controls::set_reference_position(){
	// Disables interrupts
	cli();
	// Zeroes values
	motor_position = 0;
	post_gears_position = 0;
	// Enables interrupts
	sei();
}

//-------------------------------------------------------------------------------------
/** Starts a position controller to tell the motor to move to a position desired_position
 *  degrees from the reference position.
 */

void controls::start_position_control(int desired_position){
	position_error_sum = 0;
}

void controls::start_position_control(int desired_position, int kp_val, int ki_val){
	position_error_sum = 0;
	kp = kp_val;
	ki = ki_val;
}

void controls::update_position_control(void){
	position_error = desired_position - motor_position;
	position_error_sum += position_error;
	motor_setting = position_error * kp + position_error_sum * kv;
	if(motor_setting > 255){
		motor_setting = 255;
	}
	else if(motor_setting < -255){
		motor_setting = -255;
	}

	set_power(motor_setting);
}

//-------------------------------------------------------------------------------------
/** Starts a gear position controller to tell the end of the geartrain with a predefined
 *  gear ratio
 */

void controls::start_geared_position_control(int desired_position_degrees){
	if(desired_position_degrees == 360){
		desired_gear_position = 0
	}
	else{
		desired_gear_position = (long)(desired_position_degrees * (encoder_gear_max_value + 1)) / 360;
	}
	gear_position_error_sum = 0;
}

void controls::start_geared_position_control(int desired_position_degrees, int kp_val, int ki_val){
	if(desired_position_degrees == 360){
		desired_gear_position = 0
	}
	else{
		desired_gear_position = (long)(desired_position_degrees * (encoder_gear_max_value + 1)) / 360;
	}
	gear_position_error_sum = 0;
	kp = kp_val;
	ki = ki_val;
}

void controls::update_geared_position_control(void){
	gear_position_error = desired_gear_position - gear_position;
	gear_position_error_sum += position_error;
	motor_setting = gear_position_error * kp + gear_position_error_sum * kv;
	if(motor_setting > 255){
		motor_setting = 255;
	}
	else if(motor_setting < -255){
		motor_setting = -255;
	}

	set_power(motor_setting);
}
//--------------------------------------------------------------------------------------
/** This overloaded operator allows information about the motor to be printed
 *  easily to the terminal
 */

base_text_serial& operator<< (base_text_serial& serial, controls& my_controls)
{
	// Outputs to the serial port
	serial << "kp: " << kp << "\n\rki: " << ki << "\n\rMotor power: " << power_level << endl;

	return (serial);
}
