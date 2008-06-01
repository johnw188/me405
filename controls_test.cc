//======================================================================================
/** \file controls_test.cc
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
#include <avr/io.h>
                                            // User written headers included with " "
#include "rs232.h"                          // Include header for serial port class
#include "motor_driver.h"                        // Include header for the A/D class
#include "adc_driver.h"
#include "controls.h"
#include "stl_us_timer.h"

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
    volatile unsigned long dummy, controldummy;           // Used as a not-smart delay loop counter
    char input_char;
    int position = 0;

    // Create an RS232 serial port object. Diagnostic information can be printed out 
    // using this port
    rs232 the_serial_port (BAUD_DIV, 1);
    task_timer timer;
    time_stamp zero_time(0L);
    time_stamp current_time;
    bool flag = false;

    // Creates a motor object and an adc object
    controls controller(&the_serial_port);

    // Say hello
    the_serial_port << "\r\nControls Test App\r\n";

    controller.set_kp(10);
    controller.set_ki(0);
    controller.start_geared_position_control(0);

    the_serial_port << sizeof(int) << ":" << sizeof(long) << ":" << endl;


    // Run the main scheduling loop, in which the action to run is done repeatedly.
    while (true)
    {
	    controller.update_geared_position_control();
	    if (the_serial_port.check_for_char ())
	    {
		    input_char = the_serial_port.getchar();
		    if(input_char == ' ' && flag == false)
		    {
			    position = (position + 90) % 360;
			    controller.change_gear_position(position);
			    timer.set_time(zero_time);
			    flag = true;
		    }
		    else if(input_char >= 48 && input_char <= 57){
			    controller.set_kp(input_char - 48);
			    the_serial_port << input_char - 48 << endl;
		    }
		    else if(input_char == ' ' && flag == true){
			    flag = false;
		    }
	    }
	    //if(dummy++ >= 1000 && flag){
		//    the_serial_port << timer.get_time_now() << "," << controller.get_motor_gear_position() << endl;
	    //}
    }
    return (0);
    }
