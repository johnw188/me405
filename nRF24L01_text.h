//*************************************************************************************
/** \file nRF24L01_text.h
 *      This file contains a class which operates a Nordic nRF24L01 based radio module
 *      attached to an AVR processor through the AVR's SPI port in text mode. In this
 *      context, "text mode" means that the radio will send characters and character
 *      strings as one would send strings through a serial line to a terminal, as
 *      opposed to sending packets of binary data with error checking. 
 *
 *  Notes:
 *    \li No notes yet, let's think of something
 *
 *  Revised:
 *    \li 04-22-07 MNL Original file
 *    \li 04-26-07 MNL Added a bunch of functions. Transfer verified working. 
 *    \li 05-09-07 MNL Changed so that setup() doesn't have to be called by user
 *    \li 11-15-07 SCH Overhauled to properly control radios and increase speed
 *    \li 01-26-07 SCH Fixed more major flaws
 *    \li 02-15-08 JRR Changed to use hardware SPI port on ME405 boards
 */
//*************************************************************************************

/// These defines prevent this file from being included more than once in a *.cc file
#ifndef _NRF24L01_TEXT_H_
#define _NRF24L01_TEXT_H_

#include "spi_bb.h"                         // Header for bit-banged SPI port
#include "avr_queue.h"                      // Template header for circular buffer
#include "base_text_serial.h"               // Header for base serial devices
#include "nRF24L01_base.h"                  // Header for base nRF24L01 radio driver


//-------------------------------------------------------------------------------------
/** This class operates a radio module based on a Nordic nRF24L01 chip in text mode.
 *  In this mode, the radio sends and receives character strings, acting as similarly
 *  as possible to a simple serial cable connected between two endpoints. 
 *
 *  The current version uses a bit-banged serial interface rather than the hardware
 *  serial interface on most AVR processors. This allows easier debugging of the 
 *  interface. 
 *
 *  Lines which are connected to the radio on the ME405 board, in addition to the 
 *  usual Vcc and ground (radio pin first, CPU pin second):
 *    \li MISO - MISO   - SPI data, AVR to radio
 *    \li MOSI - MISO   - SPI data, radio to AVR
 *    \li SCK  - SCK    - SPI clock line
 *    \li CSN  - SS/PB0 - SPI slave select
 *    \li CE   - PE7    - Chip enable selects transmit or receive mode
 *    \li IRQ  - PE6    - Interrupt pin (radio can interrupt CPU when data is received)
 */

class nRF24L01_text: public nRF24L01_base, public base_text_serial
    {
    // Private data and methods are accessible only from within this class and 
    // cannot be accessed from outside -- even from descendents of this class
    private:

    // Protected data and methods are accessible from this class and its descendents
    protected:
        void puts32 (const char*);          // Sends 32-char string chunks to radio

    // Public methods can be called from anywhere in the program where there is a 
    // pointer or reference to an object of this class
    public:
        // The constructor sets up the radio interface
        nRF24L01_text (volatile unsigned char&, volatile unsigned char&, unsigned char,
            volatile unsigned char&, volatile unsigned char&, unsigned char, 
            spi_bb_port*, unsigned char slave_mask, base_text_serial* = NULL);

        bool putchar (char);                // Write one character to serial port
        void puts (char const*);            // Write a string to serial port
        bool check_for_char (void);         // Check if a character is in the buffer
        char getchar (void);                // Get a character; wait if none is ready
    };

#endif  // _NRF24L01_TEXT_H_
