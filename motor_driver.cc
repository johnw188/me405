//======================================================================================
/** \file  motor_driver.cc
 *  Class which implements the functionality of the VNH3SP30 motor controller on the
 *  ME405 boards
 *
 *  Revisions:
 *    \li  04-17-08  Created files
 *    \li  04-21-08  Began implementing methods
 *    \li  04-24-08  Finished class
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>

#include "rs232.h"                          // Include header for serial port class
#include "motor_driver.h"                        // Include header for the A/D class


#define BV(bit) (1<<(bit)) //!< Byte Value => sets bit'th bit to 1
#define cbi(reg, bit) reg &= ~(BV(bit)) //!< Clears the corresponding bit in register reg
#define sbi(reg, bit) reg |= (BV(bit))  //!< Sets the corresponding bit in register reg

//-------------------------------------------------------------------------------------
/** \brief Initializes a motor driver object
 *
 *  This constructor initializes the timer registers in order to implement the PWM pin
 *  which drives the motor controller. It also sets the initial power level to zero
 *  and the initial direction to true, and sets up a serial port for debugging
 */

motor_driver::motor_driver (base_text_serial* p_serial_port)
{
	// Store the serial port locally and print a message
	ptr_to_serial = p_serial_port;
	*ptr_to_serial << "Setting up motor controller" << endl;
	power_level = 0;
	direction_of_motor = true;
	brake_on = false;
	// Initializes the timer 2 control register with prescalers to get a PWM running
	//at 120Hz
	TCCR2 = 0x6c;
	// Initializes the duty cycle to 0%
	OCR2 = 0x00;
	// Sets up data direction registers to open the relevent bits of port D and B
	DDRB = 0x80;
	DDRD = 0xa0;
	// Initializes motor to brake mode with zero input
	PORTD = 0xe0;
}


//-------------------------------------------------------------------------------------
/** \brief This method sets the power level of the motor
 *
 *   taking a number from -255 to 255 and
 *  setting the duty cycle of the PWM based upon that value. The negative flag is used
 *  to set the registers of the motor control chip to turn the motor one way or another
 *  \param  power The power level that you want to set the motor to
 *  \return True if motor is set, false if it isn't
 *  \brief Sets motor power
 */

bool motor_driver::set_power (int power)
{
	bool negative_power;
	unsigned char OCR2_value;

	if(power > 255 || -power > 255 || brake_on == true){
		// Motor not set, as brake is on or power value is wrong
		return false;
	}

	power_level = power;

	if(power < 0){
		direction_of_motor = false;
		OCR2_value = (unsigned char)-power;
	}
	else{
		direction_of_motor = true;
		OCR2_value = (unsigned char)power;
	}

	OCR2 = OCR2_value;
	PORTD &= 0x5f;
	if(direction_of_motor){
		PORTD |= 0x80;
	}
	else{
		PORTD |= 0x20;
	}
	
	// Motor set
	return true;
}

//-------------------------------------------------------------------------------------
/** \brief This method sets the power to a percentage of max power.
 *  \param  power_pct The power level that you want to set the motor to, with -100 <= power
 *  <= 100 as it's a percentage value. 
 *  \return True if motor was set correctly, false if it wasn't
 *  \brief Sets motor power
 */

bool motor_driver::set_power_pct (int power_pct)
{
	int power_value;
	power_value = (power_pct * 255) / 100;
	if(set_power(power_value)){
		return true;
	}
	else{
		return false;
	}
}

//-------------------------------------------------------------------------------------
/** \brief This method turns the brake of the motor on/off
 *  \param  brake True to set brake on, false to turn brake off
 *  \return True if brake was set, false if it was turned off
 *  \brief Sets brake
 */
bool motor_driver::set_brake (bool brake)
{
	if(brake){
		brake_on = true;
		PORTD = 0xE0;
		return true;
	}
	else{
		brake_on = false;
		set_power(power_level);
		return false;
	}
}
