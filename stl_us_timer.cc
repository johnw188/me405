//**************************************************************************************
/** \file stl_us_timer.cc
 *      This file contains a class which runs a task timer whose resolution is one
 *      microsecond. The timer is used to help schedule the execution of tasks' run()
 *      functions. The functions can be run from a main loop by checking periodically 
 *      if a task time has expired, or they can be called directly from the timer 
 *      interrupt service routine in this file, or they can be called from some other 
 *      hardware interrupt service routine; in the last case this file isn't involved. 
 *
 *  Revisions:
 *      \li 08-07-07  JRR  Created this file as daytimer.* with 1 second interrupts
 *      \li 08-08-07  JRR  Added event triggers
 *      \li 12-23-07  JRR  Made more general by allowing faster interrupt rates
 *      \li 01-05-08  JRR  Converted from time-of-day version to microsecond version
 *      \li 03-27-08  JRR  Added operators + and - for time stamps
 *      \li 03-31-08  JRR  Merged in stl_us_timer (int, long) and set_time (int, long)
 *
 *  License:
 *      This file copyright 2007 by JR Ridgely. It is released under the Lesser GNU
 *      public license, version 2.
 */
//**************************************************************************************

#include <stdlib.h>                         // Used for itoa()
#include <string.h>
#include <avr/interrupt.h>                  // There's an interrupt service routine here

#include "base_text_serial.h"               // Base for text-type serial port objects
#include "stl_us_timer.h"                   // Header for this file


//--------------------------------------------------------------------------------------
// These variables are only used to allow the interrupt service routine to update the
// measured time whenever a timer interrupt occurs, and allow a timer object to read
// the time. The user should not have any reason to read or write these variables

/** This variable holds the number of times the hardware timer has overflowed. This
 *  number is equivalent to the upper 16 bits of a 32-bit timer, and is so used. */
unsigned int ust_overflows = 0;


//--------------------------------------------------------------------------------------
/** This constructor creates a time stamp object. Nothing is put into the variables 
 *  yet; these should be filled later by getting the current time or by computing 
 *  something from time measurements and intervals. 
 */

time_stamp::time_stamp (void)
    {
    }


//--------------------------------------------------------------------------------------
/** This constructor creates a time stamp object and fills the time stamp's variables
 *  with the given values.
 *  @param a_time A 32-bit time number with which the time stamp is filled
 */

time_stamp::time_stamp (long a_time)
    {
    set_time (a_time);
    }


//--------------------------------------------------------------------------------------
/** This constructor creates a time stamp object and fills the time stamp's variables
 *  with the number of seconds and microseconds given. 
 *  @param sec A 16-bit number of seconds to preload into the time stamp
 *  @param microsec A 32-bit number of microseconds to preload into the time stamp
 */

time_stamp::time_stamp (int sec, long microsec)
    {
    set_time (sec, microsec);
    }


//--------------------------------------------------------------------------------------
/** This method fills the time stamp with the given value.
 *  @param a_time A 32-bit time number with which the time stamp is filled
 */

void time_stamp::set_time (long a_time)
    {
    data.whole = a_time;
    }


//--------------------------------------------------------------------------------------
/** This method fills the time stamp with the given numbers of seconds and microseconds.
 *  @param sec A 16-bit number of seconds to preload into the time stamp
 *  @param microsec A 32-bit number of microseconds to preload into the time stamp
 */

void time_stamp::set_time (int sec, long microsec)
    {
    data.whole = microsec / USEC_PER_COUNT;
    data.whole += sec * (1000000L / USEC_PER_COUNT);
    }


//--------------------------------------------------------------------------------------
/** This method allows one to get the time reading from this time stamp. 
 *  @param an_item A reference to a long integer in which the time stamp's data will 
 *      be put
 */

void time_stamp::get_time (long& an_item)
    {
    an_item = data.whole;
    }


//--------------------------------------------------------------------------------------
/** This method returns the number of seconds in the time stamp.
 *  @return The number of whole seconds in the time stamp
 */

int time_stamp::get_seconds (void)
    {
    return ((int)(data.whole / (1000000L / USEC_PER_COUNT)));
    }


//--------------------------------------------------------------------------------------
/** This method returns the number of microseconds in the time stamp, after the seconds
 *  are subtracted out. 
 *  @return The number of microseconds, that is, the fractional part of the time stamp
 */

long time_stamp::get_microsec (void)
    {
    return ((data.whole % 1000000L) * USEC_PER_COUNT);
    }


//--------------------------------------------------------------------------------------
/** This overloaded addition operator adds another time stamp's time to this one.  It 
 *  can be used to find the time in the future at which some event is to be caused to 
 *  happen, such as the next time a task is supposed to run. 
 *  @param addend The other time stamp which is to be added to this one
 *  @return The newly created time stamp
 */

time_stamp time_stamp::operator + (const time_stamp& addend)
    {
    time_stamp ret_stamp;
    ret_stamp.data.whole = this->data.whole + addend.data.whole;

    return ret_stamp;
    }


//--------------------------------------------------------------------------------------
/** This overloaded subtraction operator finds the duration between this time stamp's 
 *  recorded time and a previous one. 
 *  @param previous An earlier time stamp to be compared to the current one 
 *  @return The newly created time stamp
 */

time_stamp time_stamp::operator - (const time_stamp& previous)
    {
    time_stamp ret_stamp;
    ret_stamp.data.whole = this->data.whole - previous.data.whole;

    return ret_stamp;
    }


//--------------------------------------------------------------------------------------
/** This overloaded addition operator adds another time stamp's time to this one.  It 
 *  can be used to find the time in the future at which some event is to be caused to 
 *  happen, such as the next time a task is supposed to run. 
 *  @param addend The other time stamp which is to be added to this one
 */

void time_stamp::operator += (const time_stamp& addend)
    {
    data.whole += addend.data.whole;
    }


//--------------------------------------------------------------------------------------
/** This overloaded subtraction operator finds the duration between this time stamp's 
 *  recorded time and a previous one. Note that the data in this timestamp is 
 *  replaced with that duration. 
 *  @param previous An earlier time stamp to be compared to the current one 
 */

void time_stamp::operator -= (const time_stamp& previous)
    {
    data.whole -= previous.data.whole;
    }


//--------------------------------------------------------------------------------------
/** This overloaded equality test operator checks if all the time measurements in some
 *  other time stamp are equal to those in this one. 
 *  @param other A time stamp to be compared to this one 
 *  @return True if the time stamps contain equal data, false if they don't
 */

bool time_stamp::operator == (const time_stamp& other)
    {
    if (other.data.whole == data.whole)
        return (true);
    else
        return (false);
    }


//--------------------------------------------------------------------------------------
/** This overloaded inequality operator checks if this time stamp is greater than or
 *  equal to another. If the user wants to check for less-than, negating the result of
 *  this method is a lot easier (and more efficient) than writing another one. The
 *  method used to check greater-than-ness needs to work across timer overflows, so
 *  the following technique is used: subtract the other time stamp from this one as
 *  unsigned 32-bit numbers, then check if the result is positive (in which case this 
 *  time is greater) or not. 
 *  @param other A time stamp to be compared to this one 
 *  @return True if this time stamp is greater than or equal to the other one
 */

bool time_stamp::operator >= (const time_stamp& other)
    {
    long difference;                        // Vive la difference

    difference = data.whole - other.data.whole;

    if ((signed long)difference > 0L)
        return (true);
    else
        return (false);
    }


//--------------------------------------------------------------------------------------
/** This constructor creates a daytime task timer object.  It sets up the hardware timer
 *  to count at ~1 MHz and interrupt on overflow. Note that this method does not enable
 *  interrupts globally, so the user must call sei() at some point to enable the timer
 *  interrupts to run and time to actually be measured. 
 */

task_timer::task_timer (void)
    {
    #ifdef __AVR_ATmega8__                  // For the ATmega8 processor
        TCCR1A = 0x00;                      // Set to normal counting, 0 to 0xFFFF
        TCCR1B = 0x02;                      // Set prescaler to main clock / 8
        TIMSK |= 0x04;                      // Set timer 1 overflow interrupt enable
    #endif // __AVR_ATmega8__

    #ifdef __AVR_ATmega32__                 // For the ATMega32 processor
        TCCR1A = 0x00;                      // Normal counting, 0 to 0xFFFF
        TCCR1B = 0x02;                      // Set prescaler to main clock / 8
        TIMSK |= 0x04;                      // Set Timer 1 overflow interrupt enable
    #endif // __AVR_ATmega32__

    #ifdef __AVR_ATmega128__                // For the ATMega128 processor
        TCCR1A = 0x00;                      // Normal counting, 0 to 0xFFFF
        TCCR1B = 0x02;                      // Set prescaler to main clock / 8
        TIMSK |= 0x04;                      // Set Timer 1 overflow interrupt enable
    #endif // __AVR_ATmega128__

    #if defined __AVR_ATmega644__ || defined __AVR_ATmega324P__
        TCCR1A = 0x00;                      // Normal counting, 0 to 0xFFFF
        TCCR1B = 0x02;                      // Set prescaler to main clock / 8
        TCCR1C = 0x00;                      // Don't force any output compares
        TIMSK1 |= 0x01;                     // Enable the Timer 1 overflow interrupt
    #endif // __AVR_ATmega644__ or __AVR_ATmega324__
    }


//--------------------------------------------------------------------------------------
/** This method grabs the current time stamp from the hardware and overflow counters. 
 *  In order to prevent the data changing during the time when it's being read (which 
 *  would cause invalid data to be saved), interrupts are disabled while the time data 
 *  is being copied. 
 *  @param the_stamp Reference to a time stamp variable which will hold the time
 */

void task_timer::save_time_stamp (time_stamp& the_stamp)
    {
    cli ();                                 // Prevent interruption
    the_stamp.data.half[0] = TCNT1;         // Get hardware count
    the_stamp.data.half[1] = ust_overflows; // Get overflow counter data
    sei ();                                 // Re-enable interrupts
    }


//--------------------------------------------------------------------------------------
/** This method saves the current time in the internal time stamp belonging to this 
 *  object, then returns a reference to the time stamp so that the caller can use it as
 *  a measurement of what the time is now. 
 */

time_stamp& task_timer::get_time_now (void)
    {
    cli ();                                 // Prevent interruption
    now_time.data.half[0] = TCNT1;          // Get hardware count
    now_time.data.half[1] = ust_overflows;  // Get overflow counter data
    sei ();                                 // Re-enable interrupts

    return (now_time);                      // Return a reference to the current time
    }


//--------------------------------------------------------------------------------------
/** This method sets the timer to a given value. It's not likely that this method will
 *  be used, but it is provided for compatibility with other task timer implementations
 *  that measure times of day (in hours, minutes, and seconds) and do need to be set by
 *  user programs. 
 *  @param t_stamp A reference to a time stamp containing the time to be set
 */

bool task_timer::set_time (time_stamp& t_stamp)
    {
    cli ();                                 // Prevent interruption
    TCNT1 = t_stamp.data.half[0];
    ust_overflows = t_stamp.data.half[1];
    sei ();                                 // Re-enable interrupts
    }


//--------------------------------------------------------------------------------------
/** This method writes the time in seconds and microseconds into the given character 
 *  buffer. The character buffer must have space for at least 13 characters, including 
 *  the null (ASCII zero) character which marks the end of the string. 
 *  @param str A pointer to the character string buffer where the text is to go
 *  @param digits The number of digits after the decimal point to convert and display;
 *      the default value is 5, for timing to the microsecond (or timer resolution)
 */

void time_stamp::to_string (char* str, unsigned char digits)
    {
    unsigned long microseconds;             // Holds microseconds in the time
    unsigned int seconds;                   // Holds the seconds part of the time

    // The counter probably doesn't run at exactly one microsecond per count, so
    // convert the count into actual microseconds, then find the number of seconds
    // and microseconds in the time
    microseconds = (data.whole % 1000000L) * USEC_PER_COUNT;
    seconds = (int)(data.whole / (1000000L / USEC_PER_COUNT));

    utoa (seconds, str, 10);                // Put seconds in the string
    while (*str) str++;                     // Move pointer to end of string
    *str++ = '.';                           // Add the decimal point

    // For the digits in the fractional part, write 'em in backwards order. We can't
    // use itoa here because we need leading zeros
    for (char counter = 5; counter >= 0; counter--)
        {
        // Only print the digit if within "digits" range; but always divide by 10
        if (counter < digits)
            str[counter] = microseconds % 10 + '0'; 
        microseconds /= 10;
        }
    str[digits] = '\0';                     // Don't forget the end-of-string
    }


//--------------------------------------------------------------------------------------
/** This overloaded operator allows a time stamp to be printed on a serial device such
 *  as a regular serial port or radio module in text mode. This allows lines to be set
 *  up in the style of 'cout.' The timestamp is always printed as a decimal number. 
 *  @param serial A reference to the serial-type object to which to print
 *  @param stamp A reference to the time stamp to be displayed
 */

base_text_serial& operator<< (base_text_serial& serial, time_stamp& stamp)
    {
    char str[14];                           // String on which to print the time
    stamp.to_string (str, 5);               // Write the time stamp on the string
    serial << str;                          // Dump the string to the serial port

    return (serial);
    }


//--------------------------------------------------------------------------------------
/** This is the interrupt service routine which is called whenever there is a compare
 *  match on the 16-bit timer's counter. Nearly all AVR processors have a 16-bit timer
 *  called Timer 1, and this is the one we use here. 
 */

ISR (TIMER1_OVF_vect)
    {
    ust_overflows++;
    }
