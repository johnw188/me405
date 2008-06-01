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
        motor405a* p_motor;                 // Pointer to motor controller
        avr_adc* p_adc;                     // Pointer to A/D converter object
        base_text_serial* p_serial;         // Pointer to a serial port for messages

        bool going;                         // True if motor should be running now
        int angle_reading;                  // A/D reading from angle potentiometer

    public:
        // The constructor creates a new task object
        task_solenoid (time_stamp*, motor405a*, avr_adc*, base_text_serial*);

        // The run method is where the task actually performs its function
        char run (char);

        // This method is called to tell the motor to go
        void go (void);
        
        // This method is called to tell the motor to stop moving
        void stop (void);
    };
