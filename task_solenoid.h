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


//-------------------------------------------------------------------------------------
/** This class contains a task which moves a motorized lever back and forth. 
 */

class task_solenoid : public stl_task
    {
    protected:
        solenoid* ptr_solenoid;                 // Pointer to solenoid object
        base_text_serial* ptr_serial;         	// Pointer to a serial port for messages
	bool* take_picture;
	const int time_to_take_pic;		// Time to take a pic: 100 ms
	const int time_to_wake_up;		// Time to wake up: 4 min 30 sec

    public:
        // The constructor creates a new task object
        task_solenoid (time_stamp*, solenoid*, base_text_serial*);

        // The run method is where the task actually performs its function
        char run (char);
    };
