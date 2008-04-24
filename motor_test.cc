//======================================================================================
/** \file motor_test.cc
 *      This file contains a test program which checks that the motor controls on the
 *      ME405 board is working correctly. 
 *
 *  Revisions:
 *    \li  04-22-08  Wrote test class
 *    \li  04-24-08  Cleaned up, commented, implamented functionality
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

                                            // System headers included with < >
#include <stdlib.h>                         // Standard C library
                                            // User written headers included with " "
#include "rs232.h"                          // Include header for serial port class
#include "motor_driver.h"                        // Include header for the A/D class
#include "adc_driver.h"

/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal on an ATmega8 
 */
#define BAUD_DIV        52                  // For testing an ATmega128


//--------------------------------------------------------------------------------------
/** The main loop checks for user input and continuously updates the values being
 *  sent to the motor
 */

int main ()
    {
    volatile unsigned long dummy;           // Used as a not-smart delay loop counter
    bool motor_running = false;
    int power_to_motor;
    int reference_adc_value = 0;
    int power;
    char input_char;

    // Create an RS232 serial port object. Diagnostic information can be printed out 
    // using this port
    rs232 the_serial_port (BAUD_DIV, 1);

    // Creates a motor object and an adc object
    motor_driver motor(&the_serial_port);
    adc_driver duty_cycle_input(&the_serial_port);

    // Say hello
    the_serial_port << "\r\nMotor Driver Test Application\r\n";


    the_serial_port << "\r\nPress the 'n' key to set a new reference voltage to " <<
	    "the zero motor torque level. Press 's' to display the current motor status, and press space to toggle the brake\r\n" << endl;
    the_serial_port << "Brake is currently applied, press space to start the motor" << endl;

    // Run the main scheduling loop, in which the action to run is done repeatedly.
    while (true)
    {
	    if (the_serial_port.check_for_char ())
	    {
		    input_char = the_serial_port.getchar ();
		    // If space is pressed, toggle brake
		    if (input_char == ' ' && motor_running == true)
		    {
			    the_serial_port << endl << "Brake applied!" << endl;
			    motor.set_brake(true);
			    motor_running = false;
		    }
		    else if(input_char == ' ' && motor_running == false)
		    {
			    the_serial_port << endl << "Brake removed!" << endl;
			    motor.set_brake(false);
			    motor_running = true;
		    }
		    // If n is pressed, set new voltage reference value
		    else if(input_char == 'n')
		    {
			    reference_adc_value = duty_cycle_input.read_once(0);
			    the_serial_port << "New reference value set." << endl;
		    }
		    // Output motor status
		    else if(input_char == 's')
		    {
			    if(power < 0){
				    the_serial_port << "Current power = -" << -power << "%\r\n";
			    }
			    else{
				    the_serial_port << "Current power = " << power << "%\r\n";
			    }
		    }
		    else{}
	    }
	    // Continuously check the value of the adc and set the motor accordingly.
	    if(dummy++ > 100000L && motor_running){
		    dummy = 0;
		    power = duty_cycle_input.read_once(0) - reference_adc_value;
		    motor.set_power_pct(power);
	    }
    }

    return (0);
    }
