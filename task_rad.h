//*************************************************************************************
/** \file task_rad.h
 *      This file declares a task class to operate the radio transmission unit on the
 *      AVR board. It allows sending and receiving of data to and from other boards in
 *      the class.
 *
 *  \author		Ron Sloat
 *  \author		Justin Williams
 *
 *  Revisions:
 *      \li 05-28-08  Modified for use with the radio
 *	\li 06-03-08  Changed state structure, added checksum, header information
 *	\li 06-03-08  added pointer to triangulator object
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto.
 */
//*************************************************************************************
#ifndef _task_rad_H_                      // To prevent *.h file from being included
#define _task_rad_H_			// in a source file more than once

#include <stdlib.h>                         // Standard C library
#include "rs232.h"                          // Include header for serial port class
#include "stl_us_timer.h"                   // Task timer and time stamp header
#include "stl_debug.h"
#include "nRF24L01_text.h"
#include "stl_task.h"
#include "triangle.h"				// Triangulation class converts local coords to global and the other way
#include "sharp_sensor_driver.h"
#include "task_motor.h"

/** \brief A %buffer datatype to hold a packet of radio information
 */
typedef union rad_buffer
    {
    long int quad_word;                     //!< The whole 64-bit number
    char bytes[8];                          //!< The bytes in the number
    };

//-------------------------------------------------------------------------------------
/** \brief Task which sends and recieves data over the radio
 *
 *  This task implements the radio on the ME405 boards, which allows sending
 *  and receiving of data
 */

class task_rad : public stl_task
    {
    protected:
        base_text_serial* p_serial;         //!< Pointer to a serial port for messages
        nRF24L01_text* p_radio;             //!< Pointer to a radio object
	task_motor* ptr_task_motor;  //!< Pointer to a task_motor object
	sharp_sensor_driver* ptr_sharp_sensor_driver; //!< Pointer to a sharp_sensor_driver object
	triangle* ptr_triangle; //!< Pointer to a triangle object
	unsigned char count;		    //!< Count for receive/transmit array
        bool send;		    //!< True if radio is transmitting
	bool receive;		    //!< True if data has been received
	rad_buffer transmit_buffer;		    //!< 8-character transmit %buffer
	rad_buffer receive_buffer;		    //!< 8-character receive %buffer
	char ID;			//!< 1
	char packet_type;		//!< 2
	char x;				//!< 3
	char y;				//!< 4
	char a_i;			//!< 5
	char a_j;			//!< 6
	char checksum;			//!< 7
	char end_of_packet;		//!< 8
	bool sth_received;		//!< Flags that something was received	

    public:
        // The constructor creates a new task object
        task_rad (unsigned char, unsigned char, time_stamp*, nRF24L01_text*, rs232*, task_motor*, triangle*, sharp_sensor_driver*);

        // virtual run function for task stuff
        char run(char);
	
	// This method loads the transmit buffer for transmission
	void setCoords (void);

	// This method gives x-y-coords out, if true, you can receive x-value, if y you can receive y-value
	int get_coords(bool);

	// This method gets two character pointers to modify, returns 1 if data exists
	void setAngles (char, char);

	void checkSum (void);

	// tells that something was received
	bool check (void);

    };

//*************************************************************************************

#endif // _task_radio_H_
