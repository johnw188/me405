//======================================================================================
/** \file  task_motor.h
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

#ifndef _TASK_MOTOR_
#define _TASK_MOTOR_

#include <stdlib.h>
#include <avr/io.h>
#include "base_text_serial.h"
#include "stl_us_timer.h"
#include "stl_debug.h"
#include "rs232.h"
#include "stl_task.h"
#include "controls.h"

//-------------------------------------------------------------------------------------
/** \brief Defines a state machine to control the position of the motor
 */

class task_motor : public stl_task
    {
    protected:
        base_text_serial* ptr_serial; //!< Pointer to a serial port for messages
		controls* ptr_controls; //!< Pointer to a controls object
		int target_position; //!< Holds target position
		int previous_position; //!< Holds the position the system was last in
		int current_position; //!< Holds the position the system is in now
    public:
        // The constructor creates a sharp_sensor_driver task object
        task_motor(time_stamp*, base_text_serial*, controls*);

        // The run method is where the task actually performs its function
        char run(char);
	int get_target_position(void);
	int get_current_position(void);
	void increment_position(int);
	void change_position(int);
	void move_to_target(void);
	void return_to_scanning(void);
	bool position_stable(void);
	void enable_brake(void);
	void disable_brake(void);
    };

#endif
