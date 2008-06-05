//*************************************************************************************
/** \file task_radio.cc
 *      This file contains a task class for operating the radio on the AVR boards. It
 *      allows transmission and reception of data from other boards. It allows the
 *      boards to communicate with each other. Since receiving and transmiting data
 *      cannot be done at the same time, and is not mission critical, they are seperate
 *      states of the same radio task.
 *
 *  Written by:		Ron Sloat
 *  			Justin Williams
 *
 *  Revisions:
 *      \li 05-23-08  Set up task for radio control
 *	\li 06-03-08  Changed state structure, added checksum, header information
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto.
 */
//*************************************************************************************

#include "task_radio.h"


const char RESET = 0;                  	    	// State: Idle
const char SET_RX_TX = 1;                    	// State: Set RX, TX
const char RX = 2;                    	    	// State: Receiving
const char TX = 3;


//-------------------------------------------------------------------------------------
/** This constructor creates a radio task class. The radio needs pointers to the base
 *  class and the serial port for debugging.
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_rad   A pointer to a radio controller object
 *  @param p_ser   A pointer to a serial port for sending messages if required
 */

task_radio::task_radio (task_motor* p_task_motor, triangle* p_triangle, sharp_sensor_driver* p_sharp_sensor_driver, time_stamp* t_stamp, nRF24L01_text* p_rad, 
    base_text_serial* p_ser) : stl_task (*t_stamp, p_ser)
    {
    // Save pointers to other objects
	p_serial = p_ser;
	p_radio = p_rad;
	ptr_sharp_sensor_driver = p_sharp_sensor_driver;                        // Save pointers to other objects
	ptr_task_motor = p_task_motor;
	ptr_triangle = p_triangle;

    // Say hello
    p_serial->puts ("Radio task constructor\r\n");
    }


//-------------------------------------------------------------------------------------
/** This is the function which runs when it is called by the task scheduler. It allows
 *  the radio to be in three states: reset, receive, and transmit. Reset is used when
 *  starting up and won't run again, but can if needed. Receive mode polls for characters
 *  on the queue and loads them 8 at a time into the receive buffer. Transmit checks to
 *  see if the transmit flag is set and sends the data.
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 */

char task_radio::run (char state)
    {
    int count;
    switch (state)
        {
        // In State 0, reset the radio
        case (RESET):
	    p_radio->reset();
	    return (SET_RX_TX);
            break;

        // In State 1, set the radio to receive or transmit messages
        case (SET_RX_TX):
	    if (p_radio->check_for_char ())			// If characters ready...
		return (RX);					// ...change state
	    if (transmit_flag)					// If transmit ready...
		return (TX);				// ...change state
            break;

	// In State 2, receive data from the input buffer
	case (RX):
	    while (count < 8)
		{
		receive_buffer.bytes[count] = p_radio->getchar ();
		count++;
		return (SET_RX_TX);				// Return to generic state
		}
	    count = 0;
	    if (receive_buffer.bytes[6] = receive_buffer.bytes[0] + receive_buffer.bytes[1] + \
			receive_buffer.bytes[2] + receive_buffer.bytes[3] \
			+ receive_buffer.bytes[4] +  receive_buffer.bytes[5] + receive_buffer.bytes[7])
		{
		    receive_flag = 1;				// Data received ok
		}
	    else 
		{
		    receive_flag = 0;				// Data no good, disregard
		}
//	    return (STOP);
            break;
	    
        // In State 3, set the radio to transmit messages
        case (TX):
            *p_radio << transmit_buffer.quad_word;				// Send the data
	    transmit_flag = false;				// No more to send
            return (SET_RX_TX);					// Return to generic state	
            break;

        // If the state isn't a known state, call Houston; we have a problem
        default:
            STL_DEBUG_PUTS ("WARNING: Radio control task in state ");
            STL_DEBUG_WRITE (state);
            STL_DEBUG_PUTS ("\r\n");
            return (RESET);
        };

    // If we get here, no transition is called for
    return (STL_NO_TRANSITION);
    }


//-------------------------------------------------------------------------------------
/** This is the function that allows the triangulation task to send the global
 *  coordinates to the radio task, and then the function compiles the packet to send.
 *  @param char global x coord
 *  @param char global y coord
 */

void task_radio::set_data (void)
    {
int x_coord;
int y_coord;

//int triangle::angle_to_global (bool vector, signed int loc_angle, signed int distance)
x_coord = ptr_triangle->angle_to_global (1, ptr_task_motor->get_current_position(), ptr_sharp_sensor_driver->get_distance());
y_coord = ptr_triangle->angle_to_global (0, ptr_task_motor->get_current_position(), ptr_sharp_sensor_driver->get_distance());

    char checksum = ptr_triangle->get_position(1) + ptr_triangle->get_position(0) + x_coord + y_coord + '!';
    // Header preloaded from task initialization
    transmit_buffer.bytes[2] = x_coord;					// Load x coord
    transmit_buffer.bytes[3] = y_coord;					// Load y coord
    transmit_buffer.bytes[6] = checksum;				// Load checksum
    transmit_buffer.bytes[7] = '!';					// Load end character
    transmit_flag = true;					// Set flag
    *p_serial << "setdata: " << transmit_buffer.quad_word << endl;
  *p_serial << "get pos: " << ptr_triangle->get_position(1) + ptr_triangle->get_position(0)<< endl;

    }

//-------------------------------------------------------------------------------------
/** This is the function that allows the triangulation task to access data that has
 *  been received by the radio.
 *  @param char* The location of the triangulation tasks x coord
 *  @param char* The location of the triangulation tasks y coord
 *  @return 1 if data exists, 0 if not
 */

bool task_radio::get_data (char* x_p, char* y_p)
    {
    if (receive_flag)
	{
	*x_p = receive_buffer.bytes[2];				// Load the pointer with
    	*y_p = receive_buffer.bytes[3];				// the data in the buffer
	return (1);						// Data exists
	}
    receive_flag = 0;						// Reset flag
    return (0);							// Data doesn't exist
    }
