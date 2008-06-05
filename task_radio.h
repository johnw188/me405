//*************************************************************************************
/** \file task_radio.h
 *      This file declares a task class to operate the radio transmission unit on the
 *      AVR board. It allows sending and receiving of data to and from other boards in
 *      the class.
 *
 *  Written by:		Ron Sloat
 *  			Justin Williams
 *
 *  Revisions:
 *      \li 02-06-08  JRR  Original file
 *      \li 05-28-08  Modified for use with the radio
 *	\li 06-03-08  Changed state structure, added checksum, header information
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto.
 */
//*************************************************************************************


#ifndef _TASK_RADIO_
#define _TASK_RADIO_

#include <stdlib.h>
#include <avr/io.h>
#include "stl_debug.h"
#include "rs232.h"
#include "base_text_serial.h"
#include "stl_us_timer.h"
#include "stl_task.h"
#include "nRF24L01_text.h"			// Nordic nRF24L01 radio module header
#include "sharp_sensor_driver.h"
#include "task_motor.h"
#include "triangle.h"

typedef union buffer
    {
    long int quad_word;                          // The whole 64-bit number
    char bytes[8];                          	  // The bytes in the number
    };

//-------------------------------------------------------------------------------------
/** This class contains a task which moves a motorized lever back and forth.
 */

class task_radio : public stl_task
    {
    protected:
        base_text_serial* p_serial;         // Pointer to a serial port for messages
        nRF24L01_text* p_radio;             // Pointer to a radio object
	sharp_sensor_driver* ptr_sharp_sensor_driver;                        // Save pointers to other objects
	task_motor* ptr_task_motor;
	triangle* ptr_triangle;
	unsigned char count;		    // Count for receive/transmit array
        bool transmit_flag;		    // True if radio is transmitting
	bool receive_flag;		    // True if data has been received
	buffer transmit_buffer;		    // 8-character transmit buffer
	buffer receive_buffer;		    // 8-character receive buffer

    public:
        // The constructor creates a new task object
        task_radio (task_motor*, triangle*, sharp_sensor_driver*, time_stamp*, nRF24L01_text* p_rad, base_text_serial*);

	// run method
	char run (char);
	// This method loads the transmit buffer for transmission
	void set_data (void);

	// This method gets two character pointers to modify, returns 1 if data exists, else 0
	bool get_data (char*, char*);
    };

#endif
