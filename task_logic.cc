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
const char GETTING_INIT_READING = 0;
const char INIT = 1;
const char SCANNING_POSITIVE = 2;
const char SCANNING_NEGATIVE = 3;
const char GETTING_READING = 4;
const char CHANGE_DETECTED = 5;
const char FROM_RADIO = 6;

//-------------------------------------------------------------------------------------
/** This constructor creates a logic task object. It controls the logic steps in our program
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_mot   A pointer to a solenoid controller object
 *  @param p_ser   A pointer to a serial port for sending messages if required
 */
bool turning_positive = true;
bool reading_requested = false;
bool in_sensor_reading_range;
bool enable_sensor_reading = true;

task_logic::task_logic(time_stamp* t_stamp, task_solenoid* p_task_solenoid, task_sensor* p_task_sensor,
		task_motor* p_task_motor, task_rad* p_task_radio, triangle* p_triangle, base_text_serial* p_ser) : stl_task (*t_stamp, p_ser){
	ptr_task_solenoid = p_task_solenoid;
	ptr_task_sensor = p_task_sensor;
	ptr_task_motor = p_task_motor;
	ptr_task_radio = p_task_radio;
	ptr_serial = p_ser;
	ptr_triangle = p_triangle;
	ptr_serial->puts("Logic task constructor\r\n");
}

char task_logic::run(char state){

int x_temp;
int y_temp;

//	*ptr_serial << "ENTERING LOGIC TASK" << endl;
	switch(state){
		// Initialization state to get base room readings
		case(GETTING_INIT_READING):
			//ptr_serial->puts("GETTING_INIT_READING\r\n");
		*ptr_serial << "INIT READING get motor pos: " << ptr_task_motor->get_current_position() << "\r";
			if(ptr_task_motor->position_stable()){
				ptr_task_sensor->init_sensor_values();
				ptr_serial->puts("motor is stable, took an init reading\n\r");
				return(INIT);
			}
			return(GETTING_INIT_READING);
			break;
		case(INIT):
			ptr_serial->puts("INIT\n\r");
			if(ptr_task_motor->get_target_position() == 350){
				if(ptr_task_sensor->check_reading_taken()){
					turning_positive = false;
					return(SCANNING_POSITIVE);
				}
			}
			else if(ptr_task_sensor->reading_taken()){
				ptr_task_motor->increment_position(10);
				ptr_serial->puts("increment! +10\n\r");
				return(GETTING_INIT_READING);
			}
			return(INIT);
			break;
		case(SCANNING_POSITIVE):
			//ptr_serial->puts("SCANNING_POSITIVE\n\r");
			in_sensor_reading_range = (ptr_task_motor->get_current_position() % 10 < 2 || ptr_task_motor->get_current_position() % 10 > 8);
			if(in_sensor_reading_range)
				ptr_serial->puts("In Range!\n\r");
			if(enable_sensor_reading && in_sensor_reading_range){
				enable_sensor_reading = false;
				return(GETTING_READING);
			}
			else if(in_sensor_reading_range == false){
				ptr_serial->puts("Reenabling sensor reading\n\r");
				enable_sensor_reading = true;
			}
			if (ptr_task_radio->check())
				return(FROM_RADIO);
			return(STL_NO_TRANSITION);
		// When motor stabalizes, take a reading. reading_requested is a flag which prevents the
		// task from getting stuck in an infinite loop of taking readings. If the reading taken
		// is a change from the initialized reading value, go to send the coordinates over the
		// radio
		case(GETTING_READING):
			ptr_serial->puts("GETTING_READING\n\r");
			if(reading_requested == false){
				ptr_task_sensor->take_reading();
				reading_requested = true;
			}
			// Once reading is taken, if change is detected deal with it, else
			// go back to scanning
			if(ptr_task_sensor->check_reading_taken()){
				ptr_serial->puts("Checking for a change\n\r");
				reading_requested = false;
				if(ptr_task_sensor->change_detected()){
					return(CHANGE_DETECTED);
				}
				else
					return(SCANNING_POSITIVE);
			}
			return(GETTING_READING);
			break;
		case(CHANGE_DETECTED):
			ptr_serial->puts("CHANGE_DETECTED\n\r");
			ptr_task_motor->enable_brake();
			ptr_task_solenoid->take_picture();
			*ptr_serial << "after invoking take picture" << endl;
			// calculate with help of triangulation global coords and send Radio coordinates in task_radio
			ptr_task_radio->setCoords();
			if(ptr_task_solenoid->picture_done()){
				*ptr_serial << "inside if statement" << endl;
				ptr_task_motor->disable_brake();
				return(SCANNING_POSITIVE);
			}
			return(CHANGE_DETECTED);
			break;
		case(FROM_RADIO):
	
			ptr_task_motor->change_position(ptr_triangle->global_to_angle(ptr_task_radio->get_coords(1),ptr_task_radio->get_coords(0)));
			ptr_task_motor->enable_brake();
			ptr_task_solenoid->take_picture();
			*ptr_serial << "after take picture from radio" << endl;
			if(ptr_task_solenoid->picture_done()){
				*ptr_serial << "inside if statement" << endl;
				ptr_task_motor->disable_brake();
				return(SCANNING_POSITIVE);
			}
			return(FROM_RADIO);
			break;

    // If we get here, no transition is called for
    return (STL_NO_TRANSITION);
    }
}
