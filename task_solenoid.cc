//======================================================================================
/** \file  task_solenoid.cc
 *  Task which controls the solenoid to keep the camera awake and take pictures when
 *  flagged
 *
 *  Revisions:
 *    \li  05-31-08  Created file
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================
#include <stdlib.h>
#include <avr/io.h>
#include "stl_debug.h"
#include "rs232.h"
#include "stl_us_timer.h"
#include "stl_task.h"
#include "avr_adc.h"
#include "task_solenoid.h"

// S T A T E S:
const char WAITING = 0;                  		// Is waiting for change of state
const char TAKE_PIC = 1;                    		// Is taking a pic

unsigned int timer = 0;
int interval = 10000;					// interval time to run this task
const int time_to_take_pic = 100000 / interval;		// Time to take a pic: 100 ms
const int time_to_wake_up = 270000000 / interval;	// Time to wake up: 4 min 30 sec


//-------------------------------------------------------------------------------------
/** This constructor creates a motor task object. The motor object needs pointers to
 *  a solenoid controller in order to do its thing. 
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_mot   A pointer to a solenoid controller object
 *  @param p_ser   A pointer to a serial port for sending messages if required
 */

task_solenoid::task_solenoid (time_stamp* t_stamp, solenoid* p_solenoid, base_text_serial* p_ser)
    : stl_task (*t_stamp, p_ser)
    {
	ptr_solenoid = p_solenoid;                        // Save pointers to other objects
	ptr_serial = p_ser;
	take_picture

    // Say hello
    ptr_serial->puts ("Solenoid task constructor\r\n");
    }

//-------------------------------------------------------------------------------------
/** This is the function which runs when it is called by the task scheduler. It causes
 *  the solenoid to go up and down, having several states to cause such motion. 
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 */

char task_solenoid::run (char state)
    {
    switch (state)
        {
        // In State 0, the motor should stop; when it starts up again, we want it
        // to be going to the right
	case (WAITING):
        	if(take_picture){
			ptr_solenoid->turn_on();
			timer = 0;
			return(TAKE_PIC);
		}
		if (timer++ > time_to_wake_up)
			return(TAKE_PIC);
            break;

        // In State 1, the motor goes to the right
        case (TAKE_PIC):
		if (timer++ > time_to_take_pic){
			timer = 0;
			ptr_solenoid->turn_off();
			return(WAITING);
		}
            break;
        // If the state isn't a known state, call Houston; we have a problem
        default:
            STL_DEBUG_PUTS ("WARNING: Solenoid control task in state ");
            STL_DEBUG_WRITE (state);
            STL_DEBUG_PUTS ("\r\n");
            return (STOP_RIGHT);
        };
    // If we get here, no transition is called for
    return (STL_NO_TRANSITION);
    }