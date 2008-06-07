//======================================================================================
/** \file  task_motor.cc
 *  Task which controls the motor to position the camera/sensor
 *
 *  Revisions:
 *    \li  05-31-08  Created file
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include "task_motor.h"

const char INIT = 0;
const char SCANNING = 1;
const char MOVING_TO_TARGET = 2;
const char BRAKE = 3;

int delay = 1000;
bool motor_brake_flag = false;
bool move_to_target_flag = false;
//-------------------------------------------------------------------------------------
/** This constructor creates a motor task object. The motor object needs pointers to
 *  a solenoid controller in order to do its thing. 
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_mot   A pointer to a solenoid controller object
 *  @param p_ser   A pointer to a serial port for sending messages if required
 */

task_motor::task_motor (time_stamp* t_stamp, base_text_serial* p_ser, controls* p_controls) : stl_task (*t_stamp, p_ser){
	ptr_serial = p_ser;
	ptr_controls = p_controls;
	// Say hello
	ptr_serial->puts ("Motor task constructor\r\n");
	target_position = 0;
	previous_position = 0;
	current_position = 0;
	ptr_controls->start_geared_position_control(0, 7, 0);
}

//-------------------------------------------------------------------------------------
/** This is the function which runs when it is called by the task scheduler. It causes
 *  the solenoid to go up and down, having several states to cause such motion. 
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 */

char task_motor::run (char state){
//	*ptr_serial << "ENTERING MOTOR TASK" << endl;
	switch(state){
		case(INIT):
			ptr_controls->set_power_pct(30);
			return(SCANNING);
		case(SCANNING):
			current_position = get_current_position();

			if(delay != 0){
				delay -= 1;
			}
			
			if(motor_brake_flag){
				ptr_controls->set_brake(true);
				return(BRAKE);
			}

			if(move_to_target_flag){
				return(MOVING_TO_TARGET);
			}
			
			if(current_position > 350 && delay == 0){
				ptr_serial->puts("Going back\n\r");
				ptr_controls->set_power_pct(-30);
				delay = 1000;
			}
			else if(current_position < 10 && delay == 0){
				ptr_serial->puts("Going forwards\n\r");
				ptr_controls->set_power_pct(30);
				delay = 1000;
			}
			return (SCANNING);

		case(MOVING_TO_TARGET):
			ptr_controls->update_geared_position_control();
			if(move_to_target_flag = false){
				ptr_controls->set_power_pct(30);
				return(SCANNING);
			}
			return(MOVING_TO_TARGET);
			break;

		case(BRAKE):
			*ptr_serial << "in brake state" << endl;
			if(motor_brake_flag == false){
				ptr_controls->set_brake(false);
				*ptr_serial << "in brake state switching to scanning" << endl;
				return(SCANNING);
			}
			return(BRAKE);
	}
}

int task_motor::get_target_position(){
	return target_position;
}

int task_motor::get_current_position(){
	return ptr_controls->get_motor_gear_position();
}

void task_motor::increment_position(int increment){
	target_position += increment;
	ptr_controls->change_gear_position(target_position);
}

void task_motor::change_position(int angle){
	target_position = angle;
	ptr_controls->change_gear_position(target_position);
}

void task_motor::move_to_target(void){
	move_to_target_flag = true;
}

void task_motor::return_to_scanning(void){
	move_to_target_flag = false;
}

bool task_motor::position_stable(void){
	//*ptr_serial << "checking position stability, target: " << target_position << " current position: " << current_position << endl;
	if(current_position > (target_position - 2) && current_position < (target_position + 2)){
		ptr_serial->puts("STABLE!");
		return true;
	}
	return false;
}
	
void task_motor::enable_brake(void){
	*ptr_serial << "brake enabled" << endl;
	motor_brake_flag = true;
}


void task_motor::disable_brake(void){
	*ptr_serial << "brake disabled" << endl;
	motor_brake_flag = false;
	*ptr_serial << "after brake disabled" << endl;
}

