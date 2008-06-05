//======================================================================================
/** \file radio_test.cc
 *      This file contains a program to test the packet and/or text mode drivers for
 *      the nRF24L01 radio modems on the ME405 boards. 
 *
 *  Revisions
 *    \li  02-03-08  JRR  Original file, for testing text based drivers
 *    \li  03-30-08  JRR  Modified to test packet based drivers
 */
//======================================================================================

                                            // System headers included with < >
#include <stdlib.h>                         // Standard C library
#include <avr/io.h>                         // Input-output ports, special registers
#include <avr/interrupt.h>                  // Interrupt handling functions

                                            // User written headers included with " "
#include "rs232.h"                          // Serial port header
#include "nRF24L01_text.h"                  // Nordic nRF24L01 radio module header
#include "stl_us_timer.h"                   // Task timer and time stamp header
//#include "me405comm.h"			    // Package construction	


/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal */
#define BAUD_DIV        52                  // For Mega128 with 8MHz crystal


//--------------------------------------------------------------------------------------
/** The main function is the "entry point" of every C program, the one which runs first
 *  (after standard setup code has finished). For mechatronics programs, main() runs an
 *  infinite loop and never exits. 
 */

int main ()
    {
    char input_char;                        // A character typed by the user
    static unsigned long dummy = 0L;        // For sending pings only occasionally
    unsigned char ping_count = 0;           // Something sent along with a ping

    // Create a timer for the heck of it
    task_timer a_timer;
    
    // Create a serial port object. The time will be printed to this port, which
    // should be hooked up to a dumb terminal program like minicom on a PC
    rs232 the_serial (BAUD_DIV, 1);

    // Print a greeting message. This is almost always a good thing because it lets 
    // the user know that the program is actually running
    the_serial << endl << endl << "ME405: Radio Text Interface Test" << endl;

    // Create a bit-banged SPI port interface object. Masks are SCK, MISO, MOSI
    spi_bb_port my_SPI (PINB, PORTB, DDRB, 0x02, 0x04, 0x08);

    // Set up a radio module object. Parameters are port, DDR, and bitmask for each 
    // line SS, CE, and IRQ; last parameter is debugging serial port's address
    nRF24L01_text my_radio (PORTE, DDRE, 0x40, PORTE, DDRE, 0x80, &my_SPI, 0x01, 
        &the_serial);

    // Give a long, overly complex message to make sure multi-packet strings work
    my_radio << 
        "Hello, this is the radio module text mode test program. It mostly works."
        << endl;

    // Enable interrupt processing; the receiver needs this
    sei ();

    // Run the main scheduling loop, in which the action to run is done repeatedly.
    // In the future, we'll run tasks here; for now, just do things in a simple loop
    while (true)
        {
        // Check if the user has typed something. If so, either clear the counter to
        // a reading of zero time or write a carriage return and linefeed
        if (the_serial.check_for_char ())
            {
            input_char = the_serial.getchar ();
            if (input_char == 'c')                  // 'c' means carriage return
                {
                the_serial << endl;
                ping_count = 0;
                }
            else if (input_char == 'd')             // 'd' means dump registers
                my_radio.dump_regs (&the_serial);
            else if (input_char == 't')
                the_serial << "Local time: " << a_timer.get_time_now () << endl;
            else if (input_char == 'r')
                my_radio.reset ();
	    else if (input_char == 's')
                my_radio.reset ();
            }

        // Periodically transmit some nonsense
        if (++dummy > 100000)
            {
            dummy = 0L;
            my_radio << "hey was ist das fuer ein kram " << a_timer.get_time_now () << endl;
            }

        // If there's a character available in the buffer, print it
        if (my_radio.check_for_char ())
            the_serial.putchar (my_radio.getchar ());
        }

    return (0);
    }
