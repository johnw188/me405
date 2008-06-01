//*************************************************************************************
/** \file spi_bb.cc
 *      This file contains a class which allows the use of a bit-banged SPI port on
 *      an AVR microcontroller.  This allows several SPI ports on one chip, none of
 *      which has to be shared with the SPI port which is used for in-ssytem program 
 *      downloading. 
 *
 *      This code is designed to work for low-performance applications without 
 *      requiring the use of interrupts.  Interrupt based SPI port code has not 
 *      been completed or tested.
 *
 *  Revisions:
 *     \li 03-23-07  JRR  Original file
 *     \li 04-23-07  MNL  Added functions to get I/O ports from the spi_bb_port object
 *     \li 02-16-08  JRR  Changed constructor parameters from pointers to references
 */
//*************************************************************************************

#include <stdlib.h>
#include <avr/io.h>

#include "spi_bb.h"                         // Header for bit-banged SPI port code


//-------------------------------------------------------------------------------------
/** This constructor sets up a bit-banged SPI port. Such a port uses regular I/O pins, 
 *  manipulated by software, to communicate with SPI peripherals. Each port needs the
 *  pins SCK, MOSI, and MISO (clock, master out, master in) as well as one or more
 *  slave select pins, which are configured in add_slave(). 
 *  @param input_port A reference to the input port, such as PINB
 *  @param output_port A reference to the output port, such as PORTB
 *  @param ddr_port A reference to the data direction register, such as DDRB
 *  @param sck_msk Bitmask for the SCK serial clock pin
 *  @param miso_msk Bitmask for the MISO data pin
 *  @param mosi_msk Bitmask for the MOSI data pin
 */

spi_bb_port::spi_bb_port (volatile unsigned char& input_port, 
    volatile unsigned char& output_port, volatile unsigned char& ddr_port, 
    unsigned char sck_msk, unsigned char miso_msk, unsigned char mosi_msk)
    {
    char temp_char;                          // Temporary storage for 8-bit number

    inport = &input_port;                    // Save the port parameters in the 
    outport = &output_port;                  // data structure which belongs to this
    ddr = &ddr_port;                         // particular SPI port
    sck_mask = sck_msk;
    miso_mask = miso_msk;
    mosi_mask = mosi_msk;

    *outport &= ~sck_mask;                   // Leave SCK low until data is moved
    *ddr |= sck_mask;                        // Set SCK pin as an output
    *outport &= ~miso_mask;                  // Turn MISO pullup resistor off
    *ddr &= ~miso_mask;                      // Set MISO pin as an input pin
    *outport |= mosi_mask;                   // Leave MOSI at 1
    *ddr |= mosi_mask;                       // Set MOSI pin as an output
    }


//-------------------------------------------------------------------------------------
/** This method adds an SPI slave to a bit-banged SPI port. It does so by setting the
 *  given pin on the I/O port as output. This entry should be one unique bitmask that
 *  identifies one pin connected to the I/O port which is used for the other SPI pins
 *  (SCK, MISO, and MOSI). That pin is connected to the CS' (or SS') pin of the slave
 *  chip. 
 *  @param ss_mask The mask for the Slave Select (aka Chip Select) pin on slave chip
 */

void spi_bb_port::add_slave (unsigned char ss_mask)
    {
    // Set the slave select pin to be an output, and set it high (slave not selected)
    *outport |= ss_mask;
    *ddr |= ss_mask;
    }


//-------------------------------------------------------------------------------------
/** This method sends the first part of an SPI transmission that consists of a command
 *  byte plus data. It drops the slave select line, then sends one byte. It doesn't
 *  raise the slave select line again, because it's expecting more bytes to follow
 *  this one. 
 *  @param command The command byte to be transmitted over the SPI connection
 *  @param slave_mask Mask for slave select bit of device with which we're talking
 */

void spi_bb_port::exch_cmd (unsigned char* command, unsigned char slave_mask)
    {
    // Pull the appropriate slave select pin low
    *outport &= ~slave_mask;

    // Send the byte
    exch_byte (command);                    // Exchange one byte with the slave
    }


//-------------------------------------------------------------------------------------
/** This method sends the data of an SPI transmission that consists of a command byte
 *  plus data; it's meant to be called right after exch_cmd(). 
 *  @param bytes Pointer to an array holding bytes sent to and received from the device
 *  @param size The number of bytes of data to be sent and received
 *  @param slave_mask Mask for slave select bit of device with which we're talking
 */ 

void spi_bb_port::exch_data (unsigned char* bytes, char size, unsigned char slave_mask)
    {
    // Send the bytes in sequence, using 'size' as the counter
    while (size > 0)
        {
        exch_byte (bytes);           // Exchange one byte with the slave
        bytes++;                     // Go to next byte in the array
        size--;                      // Decrement array counter
        }

    // Pull the slave select and MISO pins high again, deactivating the slave
    *outport |= slave_mask | mosi_mask;
    }


//-------------------------------------------------------------------------------------
/** This method transfers bytes to and from a chip which is attached to a bit-banged 
 *  SPI port. The bytes in the given array are sent to the receiving chip, and at the 
 *  same time bytes are received from the other chip. The received bytes are put into
 *  the array which held the bytes that were sent out. 
 *  @param bytes Pointer to an array holding bytes sent to and received from the device
 *  @param size The number of bytes to be sent and received
 *  @param slave_mask Mask for slave select bit of device with which we're talking
 *      (The slave mask must have exactly one bit which is a one, and the rest zero)
 */

void spi_bb_port::transfer (unsigned char* bytes, char size, unsigned char slave_mask)
    {
    // Pull the appropriate slave select pin low
    *outport &= ~slave_mask;

    // Send the bytes in sequence, using 'size' as the counter
    while (size > 0)
        {
        exch_byte (bytes);           // Exchange one byte with the slave
        bytes++;                     // Go to next byte in the array
        size--;                      // Decrement array counter
        }

    // Pull the slave select and MISO pins high again, deactivating the slave
    *outport |= slave_mask | mosi_mask;
    }


//-------------------------------------------------------------------------------------
/** This method exchanges one byte with the SPI slave. It doesn't change the slave 
 *  select bit, as this method is expected to be called repeatedly during each 
 *  transmission/reception process unless only one byte is being exchanged. 
 *  @param byte A pointer to the single byte to be exchanged with the SPI slave
 */

void spi_bb_port::exch_byte (unsigned char* byte)
    {
    unsigned char xbitmask;                 // Masks bit being transferred

    // Transfer the bits in the byte being currently transferred, MSB first
    for (xbitmask = 0x80; xbitmask != 0; xbitmask >>= 1)
        {
        // Put the bit which is ready to be sent on the MOSI pin
        if (*byte & xbitmask) 
            *outport |= mosi_mask;
        else
            *outport &= ~mosi_mask;

        // Raise the SCK pin from 0 to 1, causing the slave to read the bit
        *outport |= sck_mask;

        // Read the bit from the peripheral into the byte being exchanged
        if (*inport & miso_mask)
            *byte |= xbitmask;
        else
            *byte &= ~xbitmask;

        // Drop the SCK pin to 0; this transfers the next bit or, for
        // the last bit in the sequence, leaves the clock signal idle
        *outport &= ~sck_mask;
        }
    }
