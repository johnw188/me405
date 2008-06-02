//======================================================================================
/** \file  task_logic.cc
 *  Task which controls the overall logic of the camera project
 *
 *  Revisions:
 *    \li  05-31-08  Created file
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include "task_logic.h"

// S T A T E S:
const char INIT = 0;          
const char GETTING_INIT_READING = 1;
const char SCANNING_POSITIVE = 2;
const char SCANNING_NEGATIVE = 3;
const char GETTING_READING = 4;
const char CHANGE_DETECTED = 5;

//-------------------------------------------------------------------------------------
/** This constructor creates a logic task object. It controls the logic steps in our program
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_mot   A pointer to a solenoid controller object
 *  @param p_ser   A pointer to a serial port for sending messages if required
 */
bool turning_positive = true;
bool reading_requested = false;

task_logic::task_logic(time_stamp* t_stamp, task_solenoid* p_task_solenoid, task_sensor* p_task_sensor,
		task_motor* p_task_motor, base_text_serial* p_ser) : stl_task (*t_stamp, p_ser){
	task_solenoid = p_task_solenoid;
	task_sensor = p_task_sensor;
	task_motor = p_task_motor;
	ptr_serial = p_ser;
	ptr_serial->puts("Setting up logic task");
}


char task_logic::run(char state){
	switch(state){
		// Initialization state to get base room readings
		case(INIT):
			if(task_motor->get_position() == 350){
				if(task_sensor->reading_taken()){
					turning_positive = false;
					return(SCANNING_NEGATIVE);
				}
			}
			else if(task_sensor->reading_taken()){
				motor_task->increment_position(10);
				return(GETTING_INIT_READING);
			}
			break;
		case(GETTING_INIT_READING):
			if(motor_task->position_stable()){
				task_sensor->init_sensor_values();
				return(INIT);
			}
			break;
		// Two "main" modes -> scanning positive and scanning negative, to cover both possible
		// directions
		case(SCANNING_POSITIVE):
			if(task_motor->get_position() == 350){
				if(task_sensor->reading_taken()){
					turning_positive = false;
					return(SCANNING_NEGATIVE);
				}
			}
			else if(task_sensor->reading_taken()){
				motor_task->increment_position(10);
				return(GETTING_READING_POSITIVE);
			}
			break;
		case(SCANNING_NEGATIVE):
			if(task_motor->get_position() == 0){
				if(task_sensor->reading_taken()){
					turning_positive = true;
					return(SCANNING_POSITIVE);
				}
			}
			else if(task_sensor->reading_taken()){
				motor_task->increment_position(-10);
				return(GETTING_READING_NEGATIVE);
			}
			break;
		// When motor stabalizes, take a reading. reading_requested is a flag which prevents the
		// task from getting stuck in an infinite loop of taking readings. If the reading taken
		// is a change from the initialized reading value, go to send the coordinates over the
		// radio
		case(GETTING_READING):
			if(motor_task->position_stable() && reading_requested == false){
				task_sensor->take_reading();
				reading_requested = true;
			}
			// Once reading is taken, if change is detected deal with it, else
			// go back to scanning
			if(task_sensor->check_reading_taken()){
				reading_requested = false;
				if(task_sensor->change_detected()){
					return(CHANGE_DETECTED);
				}
				else if(turning_positive)
					return(SCANNING_POSITIVE);
				else
					return(SCANNING_NEGATIVE);
			}
			break;
		case(CHANGE_DETECTED):
			task_solenoid->take_picture();
			if(turning_positive)
				return(SCANNING_POSITIVE);
			else
				return(SCANNING_NEGATIVE);
			break;

    // If we get here, no transition is called for
    return (STL_NO_TRANSITION);
    }

