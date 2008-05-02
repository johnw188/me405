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

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>

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

//-------------------------------------------------------------------------------------
/** This constructor initializes the data members of the controls class, and also
 *  calls the motor driver constructor to set up the motor driver variables
 */

controls::controls (base_text_serial* p_serial_port) : motor_driver(p_serial_port)
{
	// Setup interrupt pins here to trigger interrupts on rising and falling
	// edge for both encoder_pin_A and encoder_pin_B
	
	motor_position = 0;
	post_gears_position = 0;
	ki = 0;
	kp = 0;
	kd = 0;

	// Number of encoder ticks per revolution of motor
	encoder_per_rev = 2016;
	gear_ratio = 16;
	encoder_per_gear_rev = (long)encoder_per_rev * gear_ratio;
}

//-------------------------------------------------------------------------------------
/** This method sets the current position as the new reference state for angle 
 *  measurements passed to the controller
 */

void controls::set_reference_position(){
	motor_position = 0;
	post_gears_position = 0;
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
}

//-------------------------------------------------------------------------------------
/** Starts a gear position controller to tell the end of the geartrain with a predefined
 *  gear ratio
 */

void controls::start_geared_position_control(int desired_position_degrees){
	desired_gear_position = (long)(desired_position_degrees * encoder_per_gear_rev) / 360;
	gear_position_error_sum = 0;
}

void controls::start_geared_position_control(int desired_position_degrees, int kp_val, int ki_val){
	desired_gear_position = (long)(desired_position_degrees * encoder_per_gear_rev) / 360;
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
}
//--------------------------------------------------------------------------------------
/** This overloaded operator allows information about the motor to be printed
 *  easily to the terminal
 */

//base_text_serial& operator<< (base_text_serial& serial, motor_driver& my_motor)
//{
	// Outputs to the serial port
//	serial << "stuff goes here" << endl;

//	return (serial);
//}
