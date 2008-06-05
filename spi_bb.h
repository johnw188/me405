//*************************************************************************************
/** \file spi_bb.h
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

/// These defines prevent this file from being included more than once in a *.cc file
#ifndef _SPI_BB_H_
#define _SPI_BB_H_

#include "rs232.h"                          // Serial port communications header

#undef  SPI_BB_DEBUG                    /// Define this to enable debugging features


//-------------------------------------------------------------------------------------
/** This class holds the parameters and methods necessary to operate a bit-banged SPI 
 *  port. The parameters include the addresses of the input, output, and data 
 *  direction registers used as well as bitmasks that allow manipulation of the I/O
 *  pins which are used to communicate with the SPI chip(s) to which the bit-banged
 *  SPI port is attached. 
 */

class spi_bb_port
    {
    protected:
        volatile unsigned char* inport;     ///< Pointer to input port to be used
        volatile unsigned char* outport;    ///< Pointer to data direction register
        volatile unsigned char* ddr;        ///< Pointer to data direction register
        unsigned char miso_mask;            ///< Bitmask for MISO pin
        unsigned char mosi_mask;            ///< Bitmask for MOSI pin
        unsigned char sck_mask;             ///< Bitmask for SCK (serial clock) pin

    public:
        // The constructor assigns the ports for input and output and masks for bits
        spi_bb_port (volatile unsigned char&, volatile unsigned char&, 
                     volatile unsigned char&, unsigned char, unsigned char, 
                     unsigned char);

        void add_slave (unsigned char);     // Method to add a slave device connection
        void exch_byte (unsigned char*);    // Method to exchange one byte with slave

        // This method sends a first command byte to the SPI device
        void exch_cmd (unsigned char*, unsigned char);

        // This method exchanges data with the SPI device
        void exch_data (unsigned char*, char, unsigned char);

        // This method simultaneously sends and receives bytes to and from a device
        void transfer (unsigned char*, char, unsigned char);

        /** This method returns a pointer to the port used for the input line, MISO.
         */
        volatile unsigned char* get_inport (void) { return (inport); }

        /** This method returns a pointer to the port used for output on the MOSI, 
         *  SCK, and SS lines. */
        volatile unsigned char* get_outport (void) { return (outport); }

        /** This method returns a pointer to the data direction register which is used
         *  to set the directions of the bits on the input and output ports. */
        volatile unsigned char* get_ddr (void) { return (ddr); }

//      spi_bb_check_ioport ();
    };

#endif // _SPI_BB_H_
