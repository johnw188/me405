//======================================================================================
/** \file  motor_driver.cc
 *  Class which implements the functionality of the VNH3SP30 motor controller on the
 *  ME405 boards
 *
 *  Revisions:
 *    \li  04-17-08  Created files
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


/** These defines make it easier for us to manipulate the bits of our registers, by
 * creating two new commands - cbi for clear bit i and sbi for set bit i
 */

#define BV(bit) (1<<(bit)) // Byte Value => sets bit'th bit to 1
#define cbi(reg, bit) reg &= ~(BV(bit)) // Clears the corresponding bit in register reg
#define sbi(reg, bit) reg |= (BV(bit))  // Sets the corresponding bit in register reg

//-------------------------------------------------------------------------------------
/** This constructor initializes the timer registers in order to implement the PWM pin
 *  which drives the motor controller
 */

motor_driver::motor_driver (base_text_serial* p_serial_port)
{
	// Store the serial port locally and print a message
	ptr_to_serial = p_serial_port;
	*ptr_to_serial << "Setting up motor controller" << endl;
}


//-------------------------------------------------------------------------------------
/** This method 
 *  \param  channel The A/D channel which is being read must be from 0 to 7
 *  \return The result of the A/D conversion, or 0xFFFF if there was a timeout
 */

void motor_driver::set_power (int power)
{
	bool negative_power;
	unsigned char OCR2_value

	if(power < 0){
		negative_power = true;
		OCR2_value = (unsigned char)-power;
	}
	else{
		negative_power = false;
		OCR2_value = (unsigned char)power;
	}

}

//-------------------------------------------------------------------------------------
/** This method sets the power to a percentage of max power.
 *  \param  channel The A/D channel which is being read must be from 0 to 7
 *  \return The result of the A/D conversion, or 0xFFFF if there was a timeout
 */

void motor_driver::set_power_pct (int power)
{
	int power_value

}

//--------------------------------------------------------------------------------------
/** This overloaded operator allows information about the motor to be printed
 *  easily to the terminal
 */

base_text_serial& operator<< (base_text_serial& serial, avr_adc& my_adc)
{
	// Outputs to the serial port
	serial << 

	return (serial);
}
