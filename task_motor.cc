//======================================================================================
/** \file  task_motor.cc
 *  Task which %controls the motor to position the camera/sensor
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

const char INIT = 0; //!< Initializing motor
const char SCANNING = 1; //!< Scanning from side to side
const char MOVING_TO_TARGET = 2; //!< Using positional control algorithms to move to an angle
const char BRAKE = 3; //!< Brake enabled

int delay = 1000; //!< Delay required to ensure motor switches scanning direction correctly
bool motor_brake_flag = false; //!< Flag which controls the brake
bool move_to_target_flag = false; //!< Flag which controls the transition from scanning to positional control
//-------------------------------------------------------------------------------------
/** \brief This constructor creates a motor task object.
 *
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_ser   A pointer to a serial port for sending messages if required
 *  @param p_controls   A pointer to a controls object
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
 *  the motor to scan from side to side when in SCANNING mode, but when it gets put in
 *  MOVING_TO_TARGET mode it starts up a PI controller to move to a set angle. BRAKE
 *  simply sets and releases the bakes
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 *  \brief Run method for motor task
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

/** \brief Method to return the target position of the motor
 *  \return Target position
 */
int task_motor::get_target_position(){
	return target_position;
}

/** \brief Method to return the current position of the motor
 *  \return Current position
 */
int task_motor::get_current_position(){
	return ptr_controls->get_motor_gear_position();
}

/** \brief Method to increment the target position
 *  \param increment Value to increment target position by
 */
void task_motor::increment_position(int increment){
	target_position += increment;
	ptr_controls->change_gear_position(target_position);
}

/** \brief Method to change the target position of the motor
 *  \param angle Target angle for the system to move to
 */
void task_motor::change_position(int angle){
	target_position = angle;
	ptr_controls->change_gear_position(target_position);
}

/** \brief Method to enable positional control in order to move to the target position
 */
void task_motor::move_to_target(void){
	move_to_target_flag = true;
}

/** \brief Method to resume 360 degree scanning
 */
void task_motor::return_to_scanning(void){
	move_to_target_flag = false;
}

/** \brief Method to check if the system is close enough to the target position
 *  \return True if motor is within range of target_posititon
 */
bool task_motor::position_stable(void){
	//*ptr_serial << "checking position stability, target: " << target_position << " current position: " << current_position << endl;
	if(current_position > (target_position - 2) && current_position < (target_position + 2)){
		ptr_serial->puts("STABLE!");
		return true;
	}
	return false;
}

/** \brief Method to enable the brake
 */	
void task_motor::enable_brake(void){
	*ptr_serial << "brake enabled" << endl;
	motor_brake_flag = true;
}


/** \brief Method to disable the brake
 */
void task_motor::disable_brake(void){
	motor_brake_flag = false;
}

