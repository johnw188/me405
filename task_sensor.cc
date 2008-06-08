//======================================================================================
/** \file  task_sensor.cc
 *  Task which controls the acquisition of sensor readings from the sharp IR rangefinder
 *
 *  Revisions:
 *    \li  05-31-08  Created file
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================
#include "task_sensor.h"

// S T A T E S:
const char WAITING = 0;                  		//!< Is waiting for change of state
const char TAKE_READING = 1;                    	//!< Is taking a reading from ADC-Port
const char TAKE_INITIAL_READING = 2;			//!< Writes initial values to array

//-------------------------------------------------------------------------------------
/** Creates a sensor task object. This object interfaces with the sensor class to allow
 *  readings to be taken in a multitasking fashion
 *  @param t_stamp A timestamp which contains the time between runs of this task
 *  @param p_sharp_sensor_driver A pointer to a sharp_sensor_driver object
 *  @param p_task_motor   A pointer to a task_motor object
 *  @param p_ser   A pointer to a serial port for sending messages if required
 */

task_sensor::task_sensor (time_stamp* t_stamp, sharp_sensor_driver* p_sharp_sensor_driver, 
		task_motor* p_task_motor, base_text_serial* p_ser) : stl_task (*t_stamp, p_ser){
	ptr_sharp_sensor_driver = p_sharp_sensor_driver;                        // Save pointers to other objects
	ptr_serial = p_ser;
	ptr_task_motor = p_task_motor;
	take_reading_flag = false;
	reading_taken_flag = true;
	change_detected_flag = false;
	latest_reading = 0;
	// Say hello
	ptr_serial->puts ("Sensor task constructor\r\n");
}

//-------------------------------------------------------------------------------------
/** \brief Run method for the sensor task
 *  This is the function which runs when it is called by the task scheduler. If a reading
 *  is requested, it transitions into one of the two "take reading" states, one if the
 *  reading asked for was an initialization reading and the other if it was a normal reading
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 */

char task_sensor::run (char state)
{
	switch (state)
	{
		case (WAITING):
			if(take_reading_flag){
				take_reading_flag = false;
				reading_taken_flag = false;
				return(TAKE_READING);
			}
			if(take_initial_reading_flag){
				take_initial_reading_flag = false;
				return(TAKE_INITIAL_READING);
			}
			break;

			// In State 1, the sensor reads a value and compars it to the initialization
		case (TAKE_READING):
			latest_reading = ptr_sharp_sensor_driver->get_distance();
			if (ptr_sharp_sensor_driver->something_changed(ptr_task_motor->get_current_position(), latest_reading))
				change_detected_flag = true;
			reading_taken_flag = true;
			return(WAITING);
			break;
		case (TAKE_INITIAL_READING):
			ptr_sharp_sensor_driver->init_sensor_values(ptr_task_motor->get_target_position());
			reading_taken_flag = true;
			return(WAITING);
			break;
			// If the state isn't a known state, call Houston; we have a problem
		default:
			STL_DEBUG_PUTS ("WARNING: Sensor control task in state ");
			STL_DEBUG_WRITE (state);
			STL_DEBUG_PUTS ("\r\n");
			return(WAITING);
	};
	// If we get here, no transition is called for
	return (STL_NO_TRANSITION);
}

/** \brief This method is called to check if a change was detected
 *  \return True if there was a change, false if not
 */

bool task_sensor::change_detected(void){
	if(change_detected_flag){
		change_detected_flag = false;
		return true;	
	}
	else{
		return false;
	}
}

/** \brief This method is called to tell the sensor to take a reading */
void task_sensor::take_reading (void)
{
	ptr_serial->puts ("Take reading\r\n");
	take_reading_flag = true;
}

/** \brief This method is called to check if a reading has been taken
 *  \return True if reading is complete, false if not
 */
bool task_sensor::check_reading_taken(void){
	return reading_taken_flag;
}

/** \brief Checks if a reading has been taken
 *
 *  This method differs from take_reading in that it clears the reading_taken_flag
 *  as well as returning its value. This functionality is used a lot by task_logic
 *  \return True if reading is complete, false if not
 */
bool task_sensor::reading_taken(void){
	if(reading_taken_flag){
		reading_taken_flag = false;
		return true;	
	}
	else{
		return false;
	}
}

/** This method is called to tell the sensor to take an initial reading*/

void task_sensor::init_sensor_values (void)
{
	ptr_serial->puts ("Take initial reading\r\n");
	take_initial_reading_flag = true;
}
