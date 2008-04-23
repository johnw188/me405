//======================================================================================
/** \file  motor_driver.cc
 *  Class which implements the functionality of the VNH3SP30 motor controller on the
 *  ME405 boards
 *
 *  Revisions:
 *    \li  04-17-08  Created files
 *    \li  04-21-08  Began implementing methods
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
	TCCR2 = 0b01101001;
	OCR2 = 0b01111111;
	DDRD = 0b00000000;
}


//-------------------------------------------------------------------------------------
/** This method sets the power level of the motor, taking a number from -255 to 255 and
 *  setting the duty cycle of the PWM based upon that value. The negative flag is used
 *  to set the registers of the motor control chip to turn the motor one way or another
 *  \param  power The power level that you want to set the motor to
 *  \return Method doesn't return much as it's the base method of the class. More
 *  robust methods will take care of verifying that the input data is correct. If the
 *  number is invalid, the motor will simply not be set, and the function will return
 *  false. If the motor is set properly, the return will be true.
 */

bool motor_driver::set_power (int power)
{
	bool negative_power;
	unsigned char OCR2_value;

	if(power < 0){
		direction_of_motor = false;
		OCR2_value = (unsigned char)-power;
	}
	else{
		direction_of_motor = true;
		OCR2_value = (unsigned char)power;
	}

}

//-------------------------------------------------------------------------------------
/** This method sets the power to a percentage of max power.
 *  \param  power The power level that you want to set the motor to, with -100 <= power
 *  <= 100 as it's a percentage value
 *  \return True if motor was set correctly, false if it wasn't
 */

bool motor_driver::set_power_pct (int power)
{
	int power_value;

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
