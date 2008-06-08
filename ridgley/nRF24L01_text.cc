//*************************************************************************************
/** \file nRF24L01_text.cc
 *      This file contains a class which operates a Nordic nRF24L01 based radio module
 *      attached to an AVR processor through the AVR's SPI port in text mode. In this
 *      context, "text mode" means that the radio will send characters and character
 *      strings as one would send strings through a serial line to a terminal, as
 *      opposed to sending packets of binary data with error checking. 
 *
 *  Revised:
 *    \li 04-22-07 MNL Original file
 *    \li 04-26-07 MNL Added a bunch of functions. Transfer verified working. 
 *    \li 05-09-07 MNL Changed so that setup() doesn't have to be called by user
 *    \li 11-15-07 SCH Overhauled to properly control radios and increase speed
 *    \li 01-26-07 SCH Fixed more major flaws
 *    \li 02-15-08 JRR Text based version, written for use on ME405 boards
 */
//*************************************************************************************

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "nRF24L01_text.h"


/////////////////////////////////////
// unsigned char interrupt_count = 0;
/////////////////////////////////////

//-------------------------------------------------------------------------------------
/** This is a file-scope pointer to the SPI port object. It's needed by the interrupt
 *  service routine, as the ISR needs to communicate with the radio chip. Users of the
 *  radio class generally should have no need to use this pointer. */
spi_bb_port* g_p_spi;

/** This is a file-scope copy of the bitmask used to access the SPI slave select bit
 *  connected to the CSN line of the radio. The ISR uses it to talk to the radio. */
unsigned char g_slave_mask;

/** This circular buffer holds characters received from the radio. The characters can
 *  be read by calls to getchar(). */
queue<unsigned char, unsigned char, 64> g_RX_queue;


//-------------------------------------------------------------------------------------
/** This constructor sets up the nRF24L01 radio for communications. It doesn't have to
 *  do much work itself, because the constructors of its ancestors (the base nRF24L01 
 *  and base text serial port) do all the work. 
 *  @param CE_port Port for the Chip Enable (output) line
 *  @param CE_ddr Data direction register for the Chip Enable line
 *  @param CE_mask Bitmask for the Chip Enable line
 *  @param IRQ_port Port for the Interrupt ReQuest (input) line
 *  @param IRQ_ddr Data direction register for the Interrupt ReQuest line
 *  @param IRQ_mask Bitmask for the Interrupt ReQuest line 
 *  @param p_spi_port A pointer to a bit-banged SPI port connecting to the radio
 *  @param slave_mask A bitmask for the slave select (called CSN by radio) bit
 *  @param debug_port A serial port (usually RS232) for debugging text (default NULL)
 */

nRF24L01_text::nRF24L01_text (volatile unsigned char& CE_port, volatile unsigned char& 
    CE_ddr, unsigned char CE_mask, volatile unsigned char& IRQ_port, volatile unsigned 
    char& IRQ_ddr, unsigned char IRQ_mask, spi_bb_port* p_spi_port, unsigned char 
    slave_mask, base_text_serial* debug_port)
    : nRF24L01_base (CE_port, CE_ddr, CE_mask, IRQ_port, IRQ_ddr, IRQ_mask, 
        p_spi_port, slave_mask, debug_port),
    base_text_serial ()
    {
    // Enable External Interrupt 7, connected to pin PE7 and the radio's IRQ pin
    #ifdef __AVR_ATmega128__                    // For Mega128 on ME405 board
        EICRB |= (1 << ISC71); // | (1 << ISC70);   // Falling edges only
        EIMSK |= (1 << INT7);
    #else
        #error Radio interrupts currently only defined for Mega128 on ME405 board
    #endif

    // Save file-scope variables used to access the SPI port object
    g_p_spi = p_spi_port;
    g_slave_mask = slave_mask;
    }


//-------------------------------------------------------------------------------------
/** This method sends one character to the radio object's transmit buffer. Because 
 *  characters are usually sent as part of strings and we don't want to send single
 *  character packets all the time, the character is just stored in the buffer unless
 *  it causes the buffer to reach 31 bytes in length or someone has requested immediate
 *  transmission, in which case the buffer is sent. 
 *  @param chout The character to be sent out
 *  @return True if everything was OK and false if there was a timeout
 */

bool nRF24L01_text::putchar (char chout)
    {
    unsigned char payload[33];              // Buffer holds payload sent to radio
    unsigned char count;                    // Counts its way through the bytes

    // Form a buffer to be sent out via the radio module, with element 0 empty
    payload[1] = (unsigned char)chout;
    for (count = 2; count < 33; count++)
        {
        payload[count] = '\0';
        }

    // The transmit() method returns true for success
    return (transmit (payload));
    }


//-------------------------------------------------------------------------------------
/** This method writes all the characters in a string until it gets to the '\\0' at 
 *  the end. If the string is short enough, it's sent in one packet. If it's too long
 *  to fit in the current buffer, the string is split up into as many packets as are
 *  needed to get the data from here to there. 
 *  @param str The string to be written 
 */

void nRF24L01_text::puts (char const* str)
    {
    unsigned char length = strlen (str);    // Find length of string to be sent out

    // While the string is longer than 31 characters, send 31-character chunks plus
    // write payload commands and string lengths to the radio module
    while (strlen (str) > 32)
        {
        puts32 (str);
        str += 32;
        length -= 32;
        }

    // Now there are at most 32 characters left, so send them, padded with null
    // characters to make up a 32 character packet
    puts32 (str);
    }


//-------------------------------------------------------------------------------------
/** This method sends one 32-character (or less) chunk of a string to the radio module.
 *  If the chunk is less than 32 characters long, it is padded with zeros. The chunk is
 *  sent in a 33-character array because the first element will be used to hold a
 *  command byte for the radio. 
 */

void nRF24L01_text::puts32 (char const* str)
    {
    unsigned char payload[33];              // Buffer holds payload sent to radio
    unsigned char count;                    // Counts its way through the bytes
    unsigned char length = strlen (str);    // Find length of string to be sent out

    // Form a buffer to be sent out via the radio module, with element 0 empty
    for (count = 1; count <= length; count++)
        {
        payload[count] = str[count - 1];
        }
    while (count < 33)
        {
        payload[count++] = '\0';
        }
    transmit (payload);
    }


//-------------------------------------------------------------------------------------
/** This method gets one character from the serial port, if one is there.  If not, it
 *  waits until there is a character available.  This can sometimes take a long time
 *  (even forever), so use this function carefully.  One should almost always use
 *  check_for_char() to ensure that there's data available first. 
 *  @return The character which was found in the serial port receive buffer
 */

char nRF24L01_text::getchar (void)
    {
    if (!(g_RX_queue.is_empty ()))
        return (g_RX_queue.get ());

    return ('\0');
    }


//-------------------------------------------------------------------------------------
/** This function checks if there is a character in the serial port's receiver buffer.
 *  It returns 1 if there's a character available, and 0 if not. 
 *  @return True for character available, false for no character available
 */

bool nRF24L01_text::check_for_char (void)
    {
    // Check the receiver queue first; if there's something there, we're done
    if (g_RX_queue.is_empty ())
        return (false);

    return (true);
    }


//--------------------------------------------------------------------------------------
/** This is the interrupt service routine which is called whenever the nRF23L01 radio
 *  module drops its interrupt pin low. This even occurs when there's data which has
 *  arrived into the receiver. The ISR gets the data from the radio and puts it into
 *  the receiving queue. 
 */

ISR (INT7_vect)
    {
    static unsigned char buffer[33];        // Buffer holds data from radio
    unsigned char index;                    // Index into the buffer

    // Clear the interrupt flag in the processor
    EIFR |= (1 << INTF7);

    // Get data out from the buffer
    buffer[0] = nRF24_RD_PLD;
    for (unsigned char count = 1; count < 33; count++)
        buffer[count] = '\0';
    g_p_spi->transfer (buffer, 33, g_slave_mask);

    // Put data from the buffer into the queue
    for (index = 1; index <= 33; index++)
        {
        g_RX_queue.put (buffer[index]);
        if (buffer[index] == '\0') break;
        }

    // Flush the buffer
    buffer[0]= nRF24_FLUSH_RX;
    buffer[1] = 0x00;
    g_p_spi->transfer (buffer, 2, g_slave_mask);

    // Clear the interrupt source in the nRF24L01 radio 
    buffer[0] = nRF24_WR_REG | nRF24_REG_STATUS;
    buffer[1] = nRF24_RX_DR | nRF24_TX_DS | nRF24_MAX_RT;
    g_p_spi->transfer (buffer, 2, g_slave_mask);
    }
