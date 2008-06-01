//======================================================================================
/** \file mototest.cc
 *      This file contains a program to test the ME 405 board's motor driver. It makes
 *      a motor move in different directions based on the value read from a 
 *      potentiometer connected to analog channel 0. 
 *
 *  Revisions
 *    \li  01-05-08  JRR  Original file
 *    \li  02-03-08  JRR  Various cleanup, tested on new ME 405 boards
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
#include "adc_driver.h"				// A/D converter header
#include "motor_driver.h"			// Driver for motor connected to board
#include "sharp_sensor_driver.h"		// IR-Sensor
#include "stl_debug.h"				// Handy debugging macros
#include "stl_task.h"				// Base class for all task classes
#include "task_solenoid.h"			// The task that runs the motor around
//#include "task_logic.h"				// The task that makes some logic
//#include "task_motor.h"				// The task controls the motor
#include "nRF24L01_text.h"                  // Nordic nRF24L01 radio module header

/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal */
#define BAUD_DIV        52                  // For Mega128 with 8MHz crystal


//--------------------------------------------------------------------------------------
/** The main function is the "entry point" of every C program, the one which runs first
 *  (after standard setup code has finished). For mechatronics programs, main() runs an
 *  infinite loop and never exits. 
 */

int main ()
    {
    volatile unsigned int dummy = 0;        // Delay loop kind of counter
    char time_string[16];                   // Character buffer holds printable time
    char input_char;                        // A character typed by the user
    unsigned char motor_duty = 0;           // Duty cycle to send to motor
    bool going_clockwise = true;            // Which way is the motor going? 


//======================================================//
//	Create Class - Objects				//
//======================================================//

	// Create a serial port object. The time will be printed to this port, which
	// should be hooked up to a dumb terminal program like minicom on a PC
	rs232 the_serial_port (BAUD_DIV, 1);

	// Print a greeting message. This is almost always a good thing because it lets 
	// the user know that the program is actually running
	the_serial_port << "\r\n\nME405 Board Motor Test" << endl;

	// Create a really basic no-frills analog to digital converter interface object
	adc_driver my_adc (&the_serial_port);

	// Create a microsecond-resolution timer
	task_timer the_timer;

	// Create an ME405 board motor controller object
//	motor_driver my_motor (&the_serial_port);

	// Create a solenoid class-object
	solenoid mysol(&the_serial_port);

	// Create a sensor driver object
//	sharp_sensor_driver my_sharp_sensor_driver(&the_serial_port);

	// Create a bit-banged SPI port interface object. Masks are SCK, MISO, MOSI
//	spi_bb_port my_SPI (PINB, PORTB, DDRB, 0x02, 0x04, 0x08);

	// Set up a radio module object. Parameters are port, DDR, and bitmask for each 
	// line SS, CE, and IRQ; last parameter is debugging serial port's address
//	nRF24L01_text my_radio (PORTE, DDRE, 0x40, PORTE, DDRE, 0x80, &my_SPI, 0x01, &the_serial);

	// Give a long, overly complex message to make sure multi-packet strings work
//	my_radio << "Hello, this is the radio module text mode test program. It mostly works." << endl;

//======================================================//
//	Create Task - Objects				//
//======================================================//

	// Create a time stamp which holds the interval between runs of the solenoid task
	// The time stamp is initialized with a number of seconds, then microseconds
	time_stamp interval_time(0, 10000);
	the_serial_port << "Solenoid Task Interval: " << interval_time << endl;
	task_solenoid my_solenoid_task(&interval_time, &mysol, &the_serial_port);

	// Set the interval a bit slower for the user radio task (buffer gets all)
	interval_time.set_time (0, 50000);
	the_serial_port << "Radio Task Interval: " << interval_time << endl;

	// Create a Radio Task
//	task_radio my_task_radio();

	// Create a task which uses the A/D and motor controller to move a motor around
	//task_motor my_motor_task (&interval_time, &my_motor, &my_adc, &the_serial_port);

	// Turn on interrupt processing so the timer can work
	sei ();

	// Run the main scheduling loop, in which the tasks are continuously scheduled.
	// This program currently uses very simple "round robin" scheduling in which the
	// tasks are simply called in order. More sophisticated scheduling strategies
	// will be used in other more sophisticated programs
	while (true)
        {
        	//my_motor_task.schedule (the_timer.get_time_now ());
        	my_solenoid_task.schedule (the_timer.get_time_now ());
        }
    return (0);
    }
