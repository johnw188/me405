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

#include "controls.h"


/** These defines make it easier for us to manipulate the bits of our registers, by
 * creating two new commands - cbi for clear bit i and sbi for set bit i
 */

#define BV(bit) (1<<(bit)) // Byte Value => sets bit'th bit to 1
#define cbi(reg, bit) reg &= ~(BV(bit)) // Clears the corresponding bit in register reg
#define sbi(reg, bit) reg |= (BV(bit))  // Sets the corresponding bit in register reg

// Status of the encoder pins
bool ISR_encoder_pin_A, ISR_encoder_pin_B;
// Variable to hold the number of errors encountered by the encoder
int ISR_error_count;
int ISR_encoder_max_value;
long ISR_encoder_gear_max_value;
// Position of the motor (from 0-255)
unsigned int ISR_motor_position;
// Position of the ouput of the geartrain
unsigned long ISR_gear_position;
int ISR_gear_position_degrees;


/** ISR's for updating the encoder position. Increments or decrements position based upon previous state of the encoder pins
*/
ISR(INT4_vect){
	if( (PINE & 0b00010000) == 0b00010000 ){
		if(ISR_encoder_pin_A == false && ISR_encoder_pin_B == true){
			ISR_motor_position == ISR_encoder_max_value ? (ISR_motor_position = 0) : (ISR_motor_position += 1);
			ISR_gear_position == ISR_encoder_gear_max_value ? (ISR_gear_position = 0) : (ISR_gear_position += 1);
		}
		else if(ISR_encoder_pin_A == false && ISR_encoder_pin_B == false){
			ISR_motor_position == 0 ? (ISR_motor_position = ISR_encoder_max_value) : (ISR_motor_position -= 1);
			ISR_gear_position == 0 ? (ISR_gear_position = ISR_encoder_gear_max_value) : (ISR_gear_position -= 1);
		}
		else{
			ISR_error_count += 1;
		}
		ISR_encoder_pin_A = true;
	}
	else{
		if(ISR_encoder_pin_A == true && ISR_encoder_pin_B == false){
			ISR_motor_position == ISR_encoder_max_value ? (ISR_motor_position = 0) : (ISR_motor_position += 1);
			ISR_gear_position == ISR_encoder_gear_max_value ? (ISR_gear_position = 0) : (ISR_gear_position += 1);
		}
		else if(ISR_encoder_pin_A == true && ISR_encoder_pin_B == true){
			ISR_motor_position == 0 ? (ISR_motor_position = ISR_encoder_max_value) : (ISR_motor_position -= 1);
			ISR_gear_position == 0 ? (ISR_gear_position = ISR_encoder_gear_max_value) : (ISR_gear_position -= 1);
		}
		else{
			ISR_error_count += 1;
		}
		ISR_encoder_pin_A = false;
	}
}

ISR(INT5_vect){
	if( (PINE & 0b00100000) == 0b00100000 ){
		if(ISR_encoder_pin_A == false && ISR_encoder_pin_B == false){
			ISR_motor_position == ISR_encoder_max_value ? (ISR_motor_position = 0) : (ISR_motor_position += 1);
			ISR_gear_position == ISR_encoder_gear_max_value ? (ISR_gear_position = 0) : (ISR_gear_position += 1);
		}
		else if(ISR_encoder_pin_A == true && ISR_encoder_pin_B == false){
			ISR_motor_position == 0 ? (ISR_motor_position = ISR_encoder_max_value) : (ISR_motor_position -= 1);
			ISR_gear_position == 0 ? (ISR_gear_position = ISR_encoder_gear_max_value) : (ISR_gear_position -= 1);
		}
		else{
			ISR_error_count += 1;
		}
		ISR_encoder_pin_B = true;
	}
	else{
		if(ISR_encoder_pin_A == true && ISR_encoder_pin_B == true){
			ISR_motor_position == ISR_encoder_max_value ? (ISR_motor_position = 0) : (ISR_motor_position += 1);
			ISR_gear_position == ISR_encoder_gear_max_value ? (ISR_gear_position = 0) : (ISR_gear_position += 1);
		}
		else if(ISR_encoder_pin_A == false && ISR_encoder_pin_B == true){
			ISR_motor_position == 0 ? (ISR_motor_position = ISR_encoder_max_value) : (ISR_motor_position -= 1);
			ISR_gear_position == 0 ? (ISR_gear_position = ISR_encoder_gear_max_value) : (ISR_gear_position -= 1);
		}
		else{
			ISR_error_count += 1;
		}
		ISR_encoder_pin_B = false;
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

	// Setup the interrupt pins as interrupt pins triggering on any logical change
	cbi(EICRB, ISC51);
	sbi(EICRB, ISC50);
	cbi(EICRB, ISC41);
	sbi(EICRB, ISC40);
	sbi(EIMSK, INT4);
	sbi(EIMSK, INT5);

	// Zero error_count
	error_count = 0;

	// Initialize constants
	motor_position = 0;
	gear_position = 0;
	ki = 0;
	kp = 10;
	kd = 0;

	// Number of encoder ticks per revolution of motor
	encoder_max_value = 2015;
	// Gear ratio of this project
	gear_ratio = 16;
	// Number of encoder ticks to equal a full gear revolution
	encoder_gear_max_value = (long)((encoder_max_value + 1) * gear_ratio) - 1;

	// Populates ISR variables
	ISR_encoder_max_value = encoder_max_value;
	ISR_encoder_gear_max_value = encoder_gear_max_value;
	ISR_motor_position = motor_position;
	ISR_gear_position = gear_position;
	ISR_encoder_pin_A = (PORTE & 0b00010000);
	ISR_encoder_pin_B = (PORTE & 0b00100000);


	// Enable interrupts
	sei();
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
	gear_position = 0;
	// Enables interrupts
	sei();
}

//-------------------------------------------------------------------------------------
/** This method updates the class member variables with those used by the ISR
 */

void controls::update_ISR_values(){
	// Clear interrupts
	cli();
	// Updates values
	encoder_pin_A = ISR_encoder_pin_A;
	encoder_pin_B = ISR_encoder_pin_B;
	error_count = ISR_error_count;
	ISR_encoder_max_value = encoder_max_value;
	ISR_encoder_gear_max_value = encoder_gear_max_value;
	motor_position = ISR_motor_position;
	gear_position = ISR_gear_position;
	motor_position_degrees = (long)(ISR_motor_position * 360) / encoder_max_value;
	gear_position_degrees = (long)(ISR_gear_position * 360) / encoder_gear_max_value;
	// Set interrupts
	sei();
}

int controls::get_motor_gear_position(){
	return ISR_gear_position;
}
//-------------------------------------------------------------------------------------
/** Starts a position controller to tell the motor to move to a position desired_position
 *  degrees from the reference position.
 *
 *  Methods incomplete as of now, as all motor controls for this project are done via
 *  the geared_position_control methods
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
	update_ISR_values();
	position_error = desired_position - motor_position;
	position_error_sum += position_error;
	motor_setting = position_error * kp + position_error_sum * ki;
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
 *  gear ratio to move to a position
 */

void controls::start_geared_position_control(int desired_position_degrees){
	desired_gear_position = desired_position_degrees;
	gear_position_error_sum = 0;
}

void controls::start_geared_position_control(int desired_position_degrees, int kp_val, int ki_val){
	desired_gear_position = desired_position_degrees;
	gear_position_error_sum = 0;
	kp = kp_val;
	ki = ki_val;
}

void controls::update_geared_position_control(void){
	//Get position in degrees
	cli();
	ISR_gear_position_degrees = (long)(ISR_gear_position * 360) / encoder_gear_max_value;

	// Calculate error
	if(desired_gear_position > ISR_gear_position_degrees){
		gear_position_error = desired_gear_position - ISR_gear_position_degrees;
	}
	else{
		gear_position_error = -(ISR_gear_position_degrees - desired_gear_position);
	}
	sei();

	// Attempt to deal with crossing zero degrees
	if(gear_position_error > 180){
		gear_position_error -= 360;
	}
	else if(gear_position_error < -180){
		gear_position_error += 360;
	}
	
	// Integrate error
	gear_position_error_sum += gear_position_error;

	// Generate motor setting output
	motor_setting = gear_position_error * kp + gear_position_error_sum * ki;

	// Debug string
	//*ptr_to_serial << ISR_gear_position_degrees << "   " << desired_gear_position << "   " << gear_position_error << "   " << gear_position_error_sum << "   " << motor_setting << "                           \r";

	// Saturation control
	if(motor_setting > 255){
		motor_setting = 255;
	}
	else if(motor_setting < -255){
		motor_setting = -255;
	}

	// Sets motor power
	set_power(motor_setting);
}

void controls::change_gear_position(int new_position){
	// Changes gear position to a new position
	desired_gear_position = new_position;
}
//--------------------------------------------------------------------------------------
/** This overloaded operator allows information about the motor to be printed
 *  easily to the terminal
 */

base_text_serial& operator<< (base_text_serial& serial, controls& controller)
{
	// Outputs to the serial port
	serial << "kp: " << controller.get_kp() << "\n\rki: " << controller.get_ki() << "\n\rMotor position: " 
		<< controller.get_motor_position() << "\n\rGear Position: " << controller.get_motor_gear_position() 
		<< "\n\rErrors: " << controller.get_errors() << "\n\rMotor position(degrees): " 
		<< controller.get_motor_position_degrees() << "\n\rGear position(degrees): " << controller.get_gear_position_degrees() << endl;

	return (serial);
}
