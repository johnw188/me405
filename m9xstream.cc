//*************************************************************************************
/** \file m9xstream.cc
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

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include "m9xstream.h"


//-------------------------------------------------------------------------------------
/** This method sets up the 9XStream radio modem for communications.  It enables the
 *  inputs and outputs and sets the baud rate divisor, and it saves pointers to the
 *  registers which are used to operate the radio modem. Since some AVR processors
 *  have dual serial ports, this method allows one to specify a port number. 
 *  @param divisor The divisor to be used for controlling the serial port baud rate 
 *  @param CTS_port Reference to the port to which the Clear To Send line is connected
 *  @param CTS_mask Bitmask which selects the Clear To Send line on the CTS port
 *  @param port_number The number of the serial port, 0 or 1 (the second port numbered
 *      1 only exists on some processors). The default is port 0 
 */

m9XStream::m9XStream (unsigned char divisor, volatile unsigned char& CTS_port, 
    unsigned char CTS_mask, unsigned char port_number)
    : base_text_serial ()
    {
    mask_CTS = CTS_mask;                    // Save the Clear To Send bitmask
    p_CTS_port = &CTS_port;                 // Save pointer to port used for CTS bit
    CTS_port &= ~CTS_mask;                  // Set the CTS pin to be an input
    CTS_port &= ~CTS_mask;                  // Turn off the pullup on the CTS line

    if (port_number == 0)
        {
        #ifdef __AVR_AT90S2313__
            p_UDR = &UDR;
            p_USR = &USR;
            p_UCR = &UCR;
            UCR = 0x18;                     // Mode N81
            UBRR = divisor;
        #endif
        #if defined __AVR_ATmega8__ || defined __AVR_ATmega8535__ \
            || defined __AVR_ATmega32__
            p_UDR = &UDR;
            p_USR = &UCSRA;
            p_UCR = &UCSRB;
            UCSRB = 0x18;
            UCSRC = 0x86;
            UBRRH = 0x00;
            UBRRL = divisor;
        #endif
        #if defined __AVR_ATmega644__ || defined __AVR_ATmega324P__ \
            || defined __AVR_ATmega128__
            p_UDR = &UDR0;
            p_USR = &UCSR0A;
            p_UCR = &UCSR0B;
            UCSR0B = 0x18;
            UCSR0C = 0x86;
            UBRR0H = 0x00;
            UBRR0L = divisor;
        #endif
        }
    else
        {                                   // Some AVR's only have one serial port
        #if defined __AVR_AT90S2313__ || defined __AVR_ATmega8__ \
            || defined __AVR_ATmega8535__ || defined __AVR_ATmega32__
            p_UDR = NULL;
            p_USR = NULL;
            p_UCR = NULL;
        #endif
        #if defined __AVR_ATmega644__ || defined __AVR_ATmega324P__ \
            || defined __AVR_ATmega128__
            p_UDR = &UDR1;
            p_USR = &UCSR1A;
            p_UCR = &UCSR1B;
            UCSR1B = 0x18;
            UCSR1C = 0x86;
            UBRR1H = 0x00;
            UBRR1L = divisor;
        #endif
        }

//     #ifdef UART_DOUBLE_SPEED                // If double-speed macro has been defined,
//         UART_STATUS |= 0x02;                // Turn on double-speed operation
//     #endif
    }


//-------------------------------------------------------------------------------------
/** This function checks if the serial port transmitter is ready to send data.  It 
 *  tests whether transmitter buffer is empty. 
 *  @return True if the serial port is ready to send, and false if not
 */

bool m9XStream::ready_to_send (void)
    {
    // If the Clear To Send line is high, the radio modem's not ready for data
    if (*p_CTS_port & mask_CTS)
        return (false);

    // If transmitter buffer is empty, we're ready to send now
    if (*p_USR & UDRE_mask)
        return (true);

    return (false);
    }


//-------------------------------------------------------------------------------------
/** This method sends one character to the radio modem.  It waits until the radio is
 *  ready, so it can hold up the system for a while.  It times out if it waits too 
 *  long to send the character; you can check the return value to see if the character
 *  was successfully sent, or just cross your fingers and ignore the return value.
 *  Note 1:  It's possible that at slower baud rates and/or higher processor speeds, 
 *  this routine might time out even when the port is working fine.  A solution would
 *  be to change the count variable to an integer and use a larger starting number. 
 *  Note 2:  Fixed!  The count is now an integer and it works at lower baud rates.
 *  @param chout The character to be sent out
 *  @return True if everything was OK and false if there was a timeout
 */

bool m9XStream::putchar (char chout)
    {
    unsigned int count = 0;                 // Timeout counter

    // Wait for the Clear To Send line to indicate that the radio is ready for data
    for (count = 0; (*p_CTS_port & mask_CTS); count++)
        {
        if (count > UART_TX_TOUT)
            return (false);
        }

    // Now wait for the serial port transmitter buffer to be empty     
    for (count = 0; ((*p_USR & UDRE_mask) == 0); count++)
        {
        if (count > UART_TX_TOUT)
            return (false);
        }

    // The CTS line is 0 and the transmitter buffer is empty, so send the character
    *p_UDR = chout;
    return (true);
    }


//-------------------------------------------------------------------------------------
/** This method writes all the characters in a string until it gets to the '\\0' at 
 *  the end. Warning: This function blocks until it's finished; if it's necessary to
 *  maintain decent real-time performance, strings should be written one character at
 *  a time. 
 *  @param str A pointer to the string to be written 
 */

void m9XStream::puts (char const* str)
    {
    while (*str) putchar (*str++);
    }


//-------------------------------------------------------------------------------------
/** This method gets one character from the serial port, if one is there.  If not, it
 *  waits until there is a character available.  This can sometimes take a long time
 *  (even forever), so use this function carefully.  One should almost always use
 *  check_for_char() to ensure that there's data available first. 
 *  @return The character which was found in the serial port receive buffer
 */

char m9XStream::getchar (void)
    {
    //  Wait until there's something in the receiver buffer
    while ((*p_USR & RXC_mask) == 0);

    //  Return the character retreived from the buffer
    return (*p_UDR);
    }


//-------------------------------------------------------------------------------------
/** This function checks if there is a character in the serial port's receiver buffer.
 *  It returns 1 if there's a character available, and 0 if not. 
 *  @return True for character available, false for no character available
 */

bool m9XStream::check_for_char (void)
    {
    if (*p_USR & RXC_mask)
        return (true);
    else
        return (false);
    }
