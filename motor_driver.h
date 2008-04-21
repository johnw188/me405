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
/** Header for the motor_driver class. This class controls the motor driver chip on the
 *  ME405 board, by implementing a PWM via one of the ATMEGA128 timer channels
 */

class avr_adc
    {
    protected:
        // Pointer to serial port
        base_text_serial* ptr_to_serial;
	// Char to hold the current power level of the motor
	unsigned char power_level;
	// Bool to hold the current direction of the motor.
	bool direction_of_motor;

    public:
	// Initializes member variables
        motor_controller (base_text_serial*);

	// Takes an int from -255 to 255 and sets the PWM and direction accordingly
        bool set_power (int);

	// Method which takes a char from -100 to 100 and sets the duty cycle of the
	// PWM to that percentage, as well as setting the direction of the motor
	bool set_power_pct (int);
    };


//--------------------------------------------------------------------------------------
/// This operator allows status information about the motor to be written to serial
/// ports easily

base_text_serial& operator<< (base_text_serial&, avr_adc&);


#endif // _MOTOR_DRIVER_H_
