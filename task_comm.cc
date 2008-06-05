//======================================================================================
/** \file  task_comm.cc
 *  Task which controls the radioto communicate to other microcontrollers when flagged
 *
 *  Revisions:
 *    \li  05-31-08  Created file
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include "task_solenoid.h"

// S T A T E S:
const char WAITING = 0;                  		// Is waiting for change of state
const char SEND = 1;                    		// Is sending a coord
const char RECEIVE = 2					// Is receiving a packege	

unsigned int timer = 0;

//-------------------------------------------------------------------------------------
/** This constructor creates a motor task object. The motor object needs pointers to
 *  a solenoid controller in order to do its thing. 
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_mot   A pointer to a solenoid controller object
 *  @param p_ser   A pointer to a serial port for sending messages if required
 */

task_comm::task_comm (time_stamp* t_stamp, base_text_serial* p_ser)
    : stl_task (*t_stamp, p_ser)
    {
	ptr_serial = p_ser;
    // Say hello
    ptr_serial->puts ("communication task constructor\r\n");
    }

//-------------------------------------------------------------------------------------
/** This is the function which runs when it is called by the task scheduler. It causes
 *  the solenoid to go up and down, having several states to cause such motion. 
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 */

char task_comm::run (char state)
    {
    switch (state)
        {
        // In State 0, the motor should stop; when it starts up again, we want it
        // to be going to the right
	case (WAITING):
		timer++;
        	if(take_picture_flag){
			take_picture_flag = false;
			timer = 0;
			return(TAKE_PIC);
		}
		if (timer > time_to_wake_up){
			take_picture_flag = false;
			timer = 0;
			return(TAKE_PIC);
		}
            break;

        // In State 1, the motor goes to the right
        case (SEND):
		timer++;
		ptr_solenoid->turn_on();
		if (timer > time_to_take_pic){
			timer = 0;
			ptr_solenoid->turn_off();
			return(WAITING);
		}
            break;
        case (RECEIVE):
		timer++;
		ptr_solenoid->turn_on();
		if (timer > time_to_take_pic){
			timer = 0;
			ptr_solenoid->turn_off();
			return(WAITING);
		}
            break
        // If the state isn't a known state, call Houston; we have a problem
        default:
		STL_DEBUG_PUTS ("WARNING: communication control task in state ");
		STL_DEBUG_WRITE (state);
		STL_DEBUG_PUTS ("\r\n");
		return(WAITING);
        };
    // If we get here, no transition is called for
    return (STL_NO_TRANSITION);
    }

/** This method is called to tell the solenoid to take a picture
 */

void task_solenoid::take_picture (void)
    {
    ptr_serial->puts ("Taking picture\r\n");
    take_picture_flag = true;
    }
