//======================================================================================
/** \file me405project.cc
 *      This file pulls everything together to run our camera project
 *
 *  Revisions
 *    \li  04-05-08  36 hours straight in lab is no fun :(
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto. 
 */
//======================================================================================

// System headers included with < >
#include <stdlib.h>				// Standard C library
#include <avr/io.h>				// Input-output ports, special registers
#include <avr/interrupt.h>			// Interrupt handling functions

// User written headers included with " "
#include "rs232.h"				// Serial port header
#include "stl_us_timer.h"			// Microsecond-resolution timer
#include "nRF24L01_text.h"			// Nordic nRF24L01 radio module header
#include "adc_driver.h"				// A/D converter header
#include "motor_driver.h"			// Driver for motor connected to board
#include "sharp_sensor_driver.h"		// IR-Sensor
#include "stl_debug.h"				// Handy debugging macros
#include "stl_task.h"				// Base class for all task classes
#include "task_solenoid.h"			// The task that runs the motor around
#include "task_logic.h"				// The task that makes some logic
#include "task_motor.h"				// The task that controls the motor
#include "task_sensor.h"			// The task that runs the sensor
#include "triangle.h"				// Triangulation class converts local coords to global and the other way
#include "task_rad.h"

/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal */
#define BAUD_DIV        52                  // For Mega128 with 8MHz crystal


//--------------------------------------------------------------------------------------
/** \brief Main function of the project
 *
 *  This function first initializes all of the objects required to run our system, and
 *  then enters an infinite loop which schedules the tasks one after the other in
 *  a round robin fashion
 */

int main ()
    {

//======================================================//
//	Create Class - Objects				//
//======================================================//

	// Create a serial port object. The time will be printed to this port, which
	// should be hooked up to a dumb terminal program like minicom on a PC
	rs232 the_serial_port (BAUD_DIV, 1);

	// Print a greeting message. This is almost always a good thing because it lets 
	// the user know that the program is actually running
	the_serial_port << "\r\n\nME405 Camera Project" << endl;

	// Create a microsecond-resolution timer
	task_timer the_timer;

	// Create a controls object
	controls my_controls(&the_serial_port);
	
	// Create a solenoid class-object
	solenoid mysol(&the_serial_port);

	// Create a sensor driver object
	sharp_sensor_driver my_sensor(&the_serial_port);

	// Create Triangulation Class
	triangle my_triangle (&the_serial_port);
	// Set global position of camera in the room coord system (in tiles) and the angle the camera is facing
	my_triangle.set_position(6,13,0);

	//Create a bit-banged SPI port interface object. Masks are SCK, MISO, MOSI
	spi_bb_port my_SPI (PINB, PORTB, DDRB, 0x02, 0x04, 0x08);

	//Set up a radio module object. Parameters are port, DDR, and bitmask for each 
	//line SS, CE, and IRQ; last parameter is debugging serial port's address
	nRF24L01_text my_radio (PORTE, DDRE, 0x40, PORTE, DDRE, 0x80, &my_SPI, 0x01, &the_serial_port);

	//Give a long, overly complex message to make sure multi-packet strings work
	my_radio << "Hello, this is the radio module text mode test program. It mostly works." << endl;

	//======================================================//
	//	Create Task - Objects				//
	//======================================================//

	// Create a time stamp which holds the interval between runs of the solenoid task
	// The time stamp is initialized with a number of seconds, then microseconds
	time_stamp interval_time(0, 10000);
	//solenoid task
	task_solenoid my_solenoid_task(&interval_time, &mysol, &the_serial_port);
	interval_time.set_time(0,1000);
	//motor task
	task_motor my_motor_task(&interval_time, &the_serial_port, &my_controls);
	//sensor task
	task_sensor my_sensor_task(&interval_time, &my_sensor, &my_motor_task, &the_serial_port);
	//Radio Task
	task_rad my_task_radio(5, 1, &interval_time, &my_radio, &the_serial_port, &my_motor_task, &my_triangle, &my_sensor);

	// Create THE logic task which rules the world
	task_logic my_logic_task(&interval_time, &my_solenoid_task, &my_sensor_task, &my_motor_task, &my_task_radio, &my_triangle,	 &the_serial_port);

	// Turn on interrupt processing so the timer can work
	sei ();

	// Run the main scheduling loop, in which the tasks are continuously scheduled.
	// This program currently uses very simple "round robin" scheduling in which the
	// tasks are simply called in order. More sophisticated scheduling strategies
	// will be used in other more sophisticated programs
	while (true)
	{
		my_logic_task.schedule(the_timer.get_time_now());
		my_motor_task.schedule (the_timer.get_time_now ());
		my_solenoid_task.schedule (the_timer.get_time_now ());
		my_sensor_task.schedule (the_timer.get_time_now ());
		my_task_radio.schedule (the_timer.get_time_now ());
	}
	return (0);
    }

