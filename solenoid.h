//======================================================================================
/** \file  solenoid.h
 *  Header for sharp sensor driver class
 *
 *  Revisions:
 *    \li  06-01-08  Created files
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _SOLENOID_H_                         // To prevent *.h file from being included
#define _SOLENOID_H_                         // in a source file more than once


/** \brief Implements a %solenoid for taking pictures with the camera
 *  
 *  Controls a %solenoid by setting an output pin high, which is connected to
 *  a transistor, allowing current to flow through the %solenoid
 */
class solenoid
  {
  protected:
    
    base_text_serial* ptr_to_serial; //!< Pointer to serial port

    int time_for_pic; //!< Time in milliseconds to take a picture
    int time_for_focus; //!< Time in milliseconds to hit the focus of the camera, preventing it from shutting down
    
   public:
    // for debugging use a serial output
    solenoid (base_text_serial*);
    //setting time of taking picture
    bool set_pic_time (int);
    //setting time of hitting the focus
    bool set_focus_time (int);
    //taking a picture activating the solenoid for a certain amount of millisec
    void turn_on (void);
    //hit the focus of the camera for a certain amount of millisec
    void turn_off (void);
};


#endif // _SOLENOID_H_
