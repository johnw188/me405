//======================================================================================
/** \file  solenoid.cc
 *  Class which implements the functionality of the solenoid to make a picture or hit the focus
 *  ME405 boards
 *
 *  Revisions:
 *    \li  04-17-08  Created files
 *    \li  04-21-08  Began implementing methods
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include <stdlib.h>
#include <avr/io.h>
#include "rs232.h"                          // Include header for serial port class
#include "solenoid.h"

solenoid::solenoid (base_text_serial* p_serial_port)
{
  ptr_to_serial = p_serial_port;
  *ptr_to_serial << "Setting up solenid controller" << endl;
  //Sets up the data direction register to open the relevant bit of Port C
  DDRC = 0b00000001;
  //Sets the output to zero at the beginning
  PORTC = 0b00000000;
}

bool solenoid::set_pic_time (int time)
{
time_for_pic = time;
return true;
}

bool solenoid::set_focus_time (int time)
{
time_for_focus = time;
return true;
}

void solenoid::turn_on (void)
{
*ptr_to_serial << "Turning on solenoid" << endl;
    PORTC |= 0b00000001;
}

void solenoid::turn_off (void)
{
*ptr_to_serial << "Turning off solenoid" << endl;
    PORTC &= 0b00000000;
}
