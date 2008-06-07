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

#include "task_solenoid.h"

// S T A T E S:
const char WAITING = 0;                  		// Is waiting for change of state
const char TAKE_PIC = 1;                    		// Is taking a pic

unsigned int timer = 0;

int time_to_wake_up = 27000; //27000 * .01 = 270 seconds = 4 min 30 sec
int time_to_take_pic = 20; //200 * .01 = 2 sec

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
	take_picture_flag = false;
	picture_done_flag = false;
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
	//*ptr_serial << "ENTERING SOLENOID TASK" << endl;
	switch (state)
	{
		// In State 0, the motor should stop; when it starts up again, we want it
		// to be going to the right
		case (WAITING):
			//*ptr_serial << "waiting" << endl;
			timer++;
			//		*ptr_serial << "Solenoid Timer: " << timer << endl;
			if(take_picture_flag){
				//*ptr_serial << "before setting flags" << endl;
				take_picture_flag = false;
				picture_done_flag = false;
				//*ptr_serial << "after setting flags" << endl;
				timer = 0;
				//*ptr_serial << "after zeroing timer" << endl;
				return(TAKE_PIC);
			}
			if (timer > time_to_wake_up){
				take_picture_flag = false;
				timer = 0;
				return(TAKE_PIC);
			}
			return(WAITING);
			break;

			// In State 1, the motor goes to the right
		case (TAKE_PIC):
			//*ptr_serial << "taking pic" << endl;
			timer++;
			*ptr_serial << "picture timer: " << timer << endl;
			ptr_solenoid->turn_on();
			if (timer > time_to_take_pic){
				timer = 0;
				ptr_solenoid->turn_off();
				picture_done_flag = true;
				return(WAITING);
			}
			return(TAKE_PIC);
			break;
			// If the state isn't a known state, call Houston; we have a problem
		default:
			STL_DEBUG_PUTS ("WARNING: Solenoid control task in state ");
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
	//ptr_serial->puts ("Taking picture\r\n");
	take_picture_flag = true;
}

bool task_solenoid::picture_done(void){
	//*ptr_serial << "picture done?" << endl;
	//if(picture_done_flag)
	//*ptr_serial << "yes" << endl;
	if(picture_done_flag){
		*ptr_serial << "picture done flag being cleared" << endl;
		picture_done_flag = false;
		return true;
	}
	else
		return false;
}
