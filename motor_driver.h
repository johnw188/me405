//======================================================================================
/** \file  motor_driver.h
 *  Header for motor_driver class
 *
 *  Revisions:
 *    \li  04-17-08  Created files
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _MOTOR_DRIVER_H_                         // To prevent *.h file from being included
#define _MOTOR_DRIVER_H_                         // in a source file more than once


//-------------------------------------------------------------------------------------
/** This class should run the A/D converter on an AVR processor. It should have some
 *  better comments. Handing in a Doxygen file with only this would not look good. 
 */

class avr_adc
    {
    protected:
        // The ADC class needs a pointer to the serial port used to say hello
        base_text_serial* ptr_to_serial;

    public:
        // The constructor just says hello at the moment, using the serial port which
        // is specified in the pointer given to it
        motor_controller (base_text_serial*);

	// Takes an int from -255 to 255 and sets the PWM accordingly
        void set_power (int);

	// Method which takes a char from -100 to 100 and sets the duty cycle of the
	// PWM to that percentage
	void set_power_pct (int);
    };


//--------------------------------------------------------------------------------------
/// This operator allows status information about the motor to be written to serial
/// ports easily

base_text_serial& operator<< (base_text_serial&, avr_adc&);


#endif // _MOTOR_DRIVER_H_
