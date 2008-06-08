//======================================================================================
/** \file  motor_driver.h
 *  Header for motor_driver class
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

#ifndef _MOTOR_DRIVER_H_                         // To prevent *.h file from being included
#define _MOTOR_DRIVER_H_                         // in a source file more than once


//-------------------------------------------------------------------------------------
/** This class %controls the motor driver chip on the
 *  ME405 board, by implementing a PWM via one of the ATMEGA128 timer channels
 *  \brief Implements the VNH3SP30 motor driver chip
 */

class motor_driver
    {
    protected:
        base_text_serial* ptr_to_serial; //!< Pointer to serial port

	unsigned char power_level;	//!< Char to hold the current power level of the motor
	
	bool direction_of_motor; //!< Bool to hold the current direction of the motor.
	bool brake_on; //!< Holds the state of the brake
	
    public:
	// Initializes member variables
        motor_driver (base_text_serial*);

	// Takes an int from -255 to 255 and sets the PWM and direction accordingly
        bool set_power (int);

	// Method which takes a char from -100 to 100 and sets the duty cycle of the
	// PWM to that percentage, as well as setting the direction of the motor
	bool set_power_pct (int);

	bool set_brake (bool);
    };


#endif // _MOTOR_DRIVER_H_
