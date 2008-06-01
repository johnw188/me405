//*************************************************************************************
/** \file m9xstream.h
 *        This file contains a class which allows the use of a MaxStream 9XStream 
 *        radio modem with AVR microcontroller. 
 *
 *        This code is designed to work for low-performance applications without 
 *        requiring the use of interrupts.  Interrupt based receiving code has not 
 *        been completed or tested.
 *
 *  Revised:
 *      \li 04-03-06  JRR  For updated version of compiler
 *      \li 06-10-06  JRR  Ported from C++ to C for use with some C-only projects; also
 *                         the serial_avr.h header has been stuffed with defines for
 *                         compatibility among lots of AVR variants
 *      \li 08-11-06  JRR  Some bug fixes
 *      \li 03-02-07  JRR  Ported back to C++. I've had it with the limitations of C.
 *      \li 04-16-07  JO   Added write (unsigned long)
 *      \li 07-19-07  JRR  Changed some character return values to bool, added m324p
 *      \li 01-12-08  JRR  Added code for the ATmega128 using USART number 1 only
 *      \li 02-14-08  JRR  Split between base_text_serial and m9xstream files
 */
//*************************************************************************************

/// These defines prevent this file from being included more than once in a *.cc file
#ifndef _M9XSTREAM_H_
#define _M9XSTREAM_H_

#include "base_text_serial.h"               // Pull in the base class header file


//-------------------------------------------------------------------------------------
// This section contains macros to define bitmasks which have different names on 
// different processor versions. 

#ifdef __AVR_AT90S2313__                    // For the AT90S2313 processor
    #define UDRE_mask (1 << UDRE)           // Mask for transmitter empty
    #define RXC_mask (1 << RXC)             // Mask for receive complete
#endif
#ifdef __AVR_ATmega8__                      // For the ATMega8 processor
    #define UDRE_mask (1 << UDRE)           // Mask for transmitter empty
    #define RXC_mask (1 << RXC)             // Mask for receive complete
#endif
#ifdef __AVR_ATmega8535__                   // For the old ATMega8535 processor
    #define UDRE_mask (1 << UDRE)           // Mask for transmitter empty
    #define RXC_mask (1 << RXC)             // Mask for receive complete
#endif // __AVR_ATmega8535__
#ifdef __AVR_ATmega32__                     // For the ATMega32 processor
    #define UDRE_mask (1 << UDRE)           // Mask for transmitter empty
    #define RXC_mask (1 << RXC)             // Mask for receive complete
#endif // __AVR_ATmega32__
#if (defined __AVR_ATmega644__ || defined __AVR_ATmega324P__)
    #define UDRE_mask (1 << UDRE0)          // Mask for transmitter empty
    #define RXC_mask (1 << RXC)             // Mask for receive complete
#endif // ...324 or 644...
#ifdef __AVR_ATmega128__                    // For the big ATMega128 processor
    #define UDRE_mask (1 << UDRE0)          // Mask for transmitter empty
    #define RXC_mask (1 << RXC)             // Mask for receive complete
#endif // __AVR_ATmega128__


/** The number of tries to wait for the transmitter buffer to become empty */
#define UART_TX_TOUT        20000


//-------------------------------------------------------------------------------------
/** This class controls a UART (Universal Asynchronous Receiver Transmitter), a common 
 *  serial interface. It talks to old-style RS232 serial ports (through a voltage
 *  converter chip such as a MAX232) or through a USB to serial converter such as a
 *  FT232RL chip. The UART is also sometimes used to communicate directly with other
 *  microcontrollers, sensors, or wireless modems. 
 */

class m9XStream : public base_text_serial
    {
    // Protected data and methods are accessible from this class and its descendents
    // only
    protected:
        /// This is a pointer to the UART data register for the serial port.
        volatile unsigned char* p_UDR;

        /// This is a pointer to the UART status register for the serial port.
        volatile unsigned char* p_USR;

        /// This is a pointer to the control register for the serial port. 
        volatile unsigned char* p_UCR;

        /// This bitmask has a 1 for the pin connected to the Clear To Send bit. 
        unsigned char mask_CTS;

        /// Pointer to the I/O port to which the Clear To Send bit is connected.
        volatile unsigned char* p_CTS_port;

    // Public methods can be called from anywhere in the program where there is a 
    // pointer or reference to an object of this class
    public:
        // The constructor sets up the radio, saving its baud divisor and location
        m9XStream (unsigned char, volatile unsigned char&, unsigned char, 
            unsigned char = 0);

        bool ready_to_send (void);          // Check if the port is ready to transmit
        bool putchar (char);                // Write one character to serial port
        void puts (char const*);            // Write a string constant to serial port
        bool check_for_char (void);         // Check if a character is in the buffer
        char getchar (void);                // Get a character; wait if none is ready
    };

#endif  // _M9XSTREAM_H_
