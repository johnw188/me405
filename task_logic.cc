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

//-------------------------------------------------------------------------------------
/** This constructor creates a logic task object. It controls the logic steps in our program
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_mot   A pointer to a solenoid controller object
 *  @param p_ser   A pointer to a serial port for sending messages if required
 */
task_logic::task_logic(time_stamp*, solenoid*, sharp_sensor_driver*, base_text_serial*){


}


char task_logic::run(char state){
	switch(state){
		case(INIT):
			if(task_motor->get_position() == 350 && task_sensor->reading_taken()){
				return(WAITING);
			}
			else if(task_sensor->reading_taken()){
				motor_task->set_position(motor_task->get_position() + 10);
				return(GETTING_INIT_READING);
			}
			break;
		case(GETTING_INIT_READING):
			if(motor_task->position_stable()){
				task_sensor->init_sensor_values();
				return(INIT);
			}
			break;
		case(WAITING):
			if(task_sensor->change_detected()){
				send_coordinate_flag = true;
				return(TAKE_PICTURE);
			}
			else if(position_stablized_flag = true){

			}
			else if(reading_taken){
				desired_position = ((desired_position + 10) % 360);
				new_motor_position_flag = true;
				task_sensor->compare(motor_position);
			}

		case(TAKE_PICTURE):
			task_solenoid->take_picture();
			return(SEND_COORDS);
		
		case(TRIANGULATION):
			return(SEND_COORDS);

		case(SEND_COORDS):
			return(WAITING);

    // If we get here, no transition is called for
    return (STL_NO_TRANSITION);
    }

