//*************************************************************************************
/** \file nRF24L01_base.cc
 *      This file contains a class full of utility functions to work with a Nordic 
 *      nRF24L01 based radio module.
 *
 *  Revised:
 *    \li 04-22-07 MNL Original file
 *    \li 04-26-07 MNL Added a bunch of functions. Transfer verified working. 
 *    \li 05-09-07 MNL Changed so that setup() doesn't have to be called by user
 *    \li 11-15-07 SCH Overhauled to properly control radios and increase speed
 *    \li 01-26-07 SCH Fixed more major flaws
 *    \li 02-15-08 JRR Text based version, written for use on ME405 boards
 *    \li 02-17-08 JRR nRF24L01_base split off from nRF24L01_text
 */
//*************************************************************************************

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

#include "nRF24L01_base.h"


//-------------------------------------------------------------------------------------
/** This constructor sets up the nRF24L01 radio for communications. It configures bits
 *  in I/O ports which are used for communication with the radio module, and it saves
 *  ports and bitmasks so that other functions can talk to the radio later. 
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

nRF24L01_base::nRF24L01_base (volatile unsigned char& CE_port, volatile unsigned char& 
    CE_ddr, unsigned char CE_mask, volatile unsigned char& IRQ_port, volatile unsigned 
    char& IRQ_ddr, unsigned char IRQ_mask, spi_bb_port* p_spi_port, unsigned char 
    slave_mask, base_text_serial* debug_port)
    {
    port_CE = &CE_port;                     // Save CE port
    mask_CE = CE_mask;                      // Save CE bitmask
    CE_ddr |= CE_mask;                      // Set CE pin as an output
    port_IRQ = &IRQ_port;                   // Save IRQ port
    mask_IRQ = IRQ_mask;                    // Save IRQ bitmask
    IRQ_ddr &= ~IRQ_mask;                   // Set IRQ pin as an input
    p_spi = p_spi_port;                     // Save pointer to SPI port
    p_serport = debug_port;                 // Save pointer to serial port
    slave_msk = slave_mask;                 // Save bitmask for slave select
    p_spi_port->add_slave (slave_mask);     // Configure the slave select output

    reset ();                               // Set radio module to starting state
    }


//-------------------------------------------------------------------------------------
/** This method sets the number of bytes of payload the receiver will be expecting. 
 *  For first versions, we'll be setting this at 32 bytes for all uses. 
 *  @param bytes The number of bytes to be expected in the payload
 *  @param pipe The number of the pipe whose payload width is to be set (default 0)
 */

void nRF24L01_base::set_payload_width (unsigned char bytes, unsigned char pipe)
    {
    unsigned char cmd[2];                   // Temporary storage for commands & data

    if (pipe > 6) return;                   // Pipe numbers over 6 aren't legal
    if (bytes > 32) return;                 // Maximum packet size is 32 bytes

    cmd[0] = nRF24_WR_REG | (nRF24_REG_PW_P0 + pipe);
    cmd[1] = bytes;
    p_spi->transfer (cmd, 2, slave_msk);
    }


//-------------------------------------------------------------------------------------
/** This method puts the radio into a standard transmission mode. Generally this will
 *  be done just long enough to send a packet, then things go back to receive mode, or
 *  sometimes it's nap time. 
 */

void nRF24L01_base::set_transmit_mode (void)
    {
    unsigned char cmd[2];                   // Temporary storage for commands & data

    cmd[0] = nRF24_WR_REG | nRF24_REG_CONF;
    cmd[1] = nRF24_XMIT_MODE;
    p_spi->transfer (cmd, 2, slave_msk);

    *port_CE &= ~mask_CE;                   // Turn the receiver off by dropping CE
    }


//-------------------------------------------------------------------------------------
/** This method puts the radio into a standard receiving mode. It will wait for 
 *  packets to arrive and alert the microcontroller using the IRQ line when one has 
 *  arrived. Other interrupt sources (transmitter empty and too many retries) are
 *  masked so they won't cause interrupts. 
 */

void nRF24L01_base::set_receive_mode (void)
    {
    unsigned char cmd[2];                   // Temporary storage for commands & data

    cmd[0] = nRF24_WR_REG | nRF24_REG_CONF;
    cmd[1] = nRF24_RECV_MODE;
    p_spi->transfer (cmd, 2, slave_msk);

    *port_CE |= mask_CE;                    // Receiver needs CE high to be on
    }


//-------------------------------------------------------------------------------------
/** This method checks if there's data available to be read.
 *  @return True if there's data available in the buffer and false if not
 */

bool nRF24L01_base::data_ready (void)
    {
    unsigned char cmd[2];                   // Temporary space for commands & data

    cmd[0] = nRF24_RD_REG | nRF24_REG_STATUS;
    p_spi->transfer (cmd, 2, slave_msk);

    if (cmd[0] & nRF24_RX_DR)
        return (true);

    return (false);
    }


//-------------------------------------------------------------------------------------
/** This method transmits a packet of data using the radio. Data sent to this method
 *  should be in an array of 33 unsigned characters; the first element in the array
 *  will be discarded (it will be used to hold the Write Payload command for the radio)
 *  and the other 32 elements contain data to be sent over the radio. 
 *  @param buffer A pointer to an array of 33 unsigned chars, the first one expendable
 *  @return True if the data was successfully sent, false if not
 */

bool nRF24L01_base::transmit (unsigned char* buffer)
    {
    unsigned char cmd[2];                   // Smaller buffer for commands & configs
    bool success = true;                    // Success flag to be returned by this fn.

    set_transmit_mode ();                   // Turn off the receiver for a moment

    // Make sure the radio's ready to send some data
    

    // Flush transmitter buffer, then put the data to send in the buffer
    cmd[0] = nRF24_FLUSH_TX;
    cmd[1] = 0x00;
    p_spi->transfer (cmd, 2, slave_msk);

    buffer[0] = nRF24_WR_PLD;
    p_spi->transfer (buffer, 33, slave_msk);

    // Pulse the CE line to begin the transmission; the pulse must be >10 us long
    *port_CE |= mask_CE;
    for (volatile unsigned int blah = 0; blah < 100; blah++);
    *port_CE &= ~mask_CE;

    // Check for success -- what happened? Look for acknowledgement of reception
    for (unsigned int timeout = 0; ; timeout++)
        {
        cmd[0] = nRF24_RD_REG | nRF24_REG_STATUS;
        cmd[1] = 0x00;
        p_spi->transfer (cmd, 2, slave_msk);

        if (cmd[1] & nRF24_TX_DS)           // It worked OK! Success remains true
            break;
        if (cmd[1] & nRF24_MAX_RT)          // No acknowledgement, phooey
            {
            success = false;
            break;
            }
        if (timeout > 1000)                 // Waited too long for acknowledgement
            {
            success = false;
            break;
            }
        }

    // Re-flush transmitter buffer
//     cmd[0] = nRF24_FLUSH_TX;
//     cmd[1] = 0x00;
//     p_spi->transfer (cmd, 2, slave_msk);

    set_receive_mode ();                    // Turn the receiver back on again
    
    return (success);
    }


//-------------------------------------------------------------------------------------
/** This method resets the radio. It can be used if the radio gets into an unknown
 *  state. It sets the following configuration:
 *    \li Only the data received interrupt is used; others are masked off
 *    \li Auto-acknowledgement and re-transmission are disabled for all pipes
 *    \li Automatic retrying of sending is turned off
 *    \li Set send and receive payload widths to 32 bytes
 */

void nRF24L01_base::reset (void)
    {
    unsigned char cmd[2];                   // Temporary storage for commands/data

    // Mask off unneeded interrupts and put radio in receive mode
    cmd[0] = nRF24_WR_REG | nRF24_REG_CONF;
    cmd[1] = nRF24_RECV_MODE;
    p_spi->transfer (cmd, 2, slave_msk);

    // Disable auto-acknowledgement for text mode
    cmd[0] = nRF24_WR_REG | nRF24_REG_EN_AA;
    cmd[1] = 0x00;                          // Changed 02-20-08: Ack turned off
    p_spi->transfer (cmd, 2, slave_msk);

    // Turn on/off automatic retries
    cmd[0] = nRF24_WR_REG | nRF24_REG_SETUP_RETR;
    cmd[1] = 0x00;                          // Changed 02-20-08: Retries turned off
    p_spi->transfer (cmd, 2, slave_msk);

    // Set address width to 4 bytes
    cmd[0] = nRF24_WR_REG | nRF24_REG_SETUP_AW;
    cmd[1] = 0x02;
    p_spi->transfer (cmd, 2, slave_msk);

    // Set RF setup register: Max power, low data rate, LNA (Low Noise Amp) gain ??
    cmd[0] = nRF24_WR_REG | nRF24_REG_RF_SETUP;
    cmd[1] = 0x0F;
    p_spi->transfer (cmd, 2, slave_msk);

    // Set receiver payload width for Pipe 0 to 32 bytes
    set_payload_width (32, 0);

    // Clear all the interrupt sources
    cmd[0] = nRF24_WR_REG | nRF24_REG_STATUS;
    cmd[1] = nRF24_TX_DS | nRF24_RX_DR | nRF24_MAX_RT | nRF24_TX_FULL;
    p_spi->transfer (cmd, 2, slave_msk);

    // Flush the transmit and receive buffers
    cmd[0] = nRF24_FLUSH_TX;
    cmd[1] = 0x00;
    p_spi->transfer (cmd, 2, slave_msk);

    cmd[0]= nRF24_FLUSH_RX;
    cmd[1] = 0x00;
    p_spi->transfer (cmd, 2, slave_msk);

    // Leave the CE line high to keep the receiver on
    set_receive_mode ();                    // Turn the receiver back on again
    }


//-------------------------------------------------------------------------------------
/** This method sets the transmitter's address. If 3 or 4 byte addresses are going to 
 *  be used, the array given to this command should be padded to 5 bytes.
 *  @param addr The address to be set, in a five-byte character array
 *  FIXME:  This should be changed to a four-byte address passed in a long int
 */

void nRF24L01_base::set_TX_address (unsigned char* addr)
    {
    unsigned char bytes[6];                 // Array to be sent via SPI to radio

    // Fill the array with write-register command and the address data
    bytes[0] = nRF24_WR_REG | nRF24_REG_TX_ADDR;
    for (unsigned char count = 1; count <= 5; count++)
        bytes[count] = addr[count];

    p_spi->transfer (bytes, 6, slave_msk);
    }


//-------------------------------------------------------------------------------------
/** This method sets the transmitter's address. If 3 or 4 byte addresses are going to 
 *  be used, the array given to this command should be padded to 5 bytes.
 *  @param addr The address to be set, in a five-byte character array
 *  @param pipe The number of the pipe whose address will be set (default 0)
 *  FIXME:  This should be changed to a four-byte address passed in a long int
 */

void nRF24L01_base::set_RX_address (unsigned char* addr, unsigned char pipe)
    {
    unsigned char bytes[6];                 // Array to be sent via SPI to radio

    // Fill the array with write-register command and the address data
    bytes[0] = nRF24_WR_REG | nRF24_REG_RX_ADDR_P0;
    for (unsigned char count = 1; count <= 5; count++)
        bytes[count] = addr[count];

    // Send the information over the SPI connection
    if (pipe == 0)
        p_spi->transfer (bytes, 6, slave_msk);

    // TODO: If not pipe 0, only send part of the address (?) need to check this out
    }


//-------------------------------------------------------------------------------------
/** This function checks if the radio transmitter is ready to send data. 
 *  @return True if the serial port is ready to send, and false if not
 */

bool nRF24L01_base::ready_to_send (void)
    {
    // Still wondering how to make this work... ///////////////////////////////////////
    return (true);
    }


//-------------------------------------------------------------------------------------
/** This method displays the contents of the nRF24L01's registers on the given serial
 *  device in a convenient, easy to read (for nerds) format.
 *  @param p_serial A pointer to the serial port device on which to display registers
 *  @param base Base in which results are displayed (bin, oct, dec, hex; default hex)
 */

void nRF24L01_base::dump_regs (base_text_serial* p_serial, ser_manipulator base)
    {
    unsigned char reg_data[6];

    *p_serial << "Registers in nRF24L01:" << base << endl;

    reg_data[0] = nRF24_REG_CONF;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "Config:   " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_EN_AA;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "Auto Ack: " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_EN_RXADDR;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "Pipes En: " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_SETUP_AW;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "Addr Wid: " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_SETUP_RETR;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "Retry:    " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_RF_CH;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "RF Chan:  " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_RF_SETUP;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "RF Setup: " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_STATUS;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "Status:   " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_OBS_TX;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "TX Errs:  " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_CD;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "Carrier:  " << reg_data[1] << endl;

    reg_data[0] = nRF24_REG_RX_ADDR_P0;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 6, slave_msk);
    *p_serial << "P0 Addr:  " << hex << reg_data[1] << "." << reg_data[2] << "." 
        << reg_data[3] << "." << reg_data[4] << "." << reg_data[5] << endl << base;

    reg_data[0] = nRF24_REG_TX_ADDR;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 6, slave_msk);
    *p_serial << "TX Addr:  " << hex << reg_data[1] << "." << reg_data[2] << "." 
        << reg_data[3] << "." << reg_data[4] << "." << reg_data[5] << endl << base;

    reg_data[0] = nRF24_REG_PW_P0;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "P0 Width: " << reg_data[1] << " (" << dec << reg_data[1] << ")" 
        << endl << base;

    reg_data[0] = nRF24_REG_FIFO_STATUS;
    reg_data[1] = 0x00;
    p_spi->transfer (reg_data, 2, slave_msk);
    *p_serial << "FIFO:     " << reg_data[1] << endl;

    *p_serial << endl;
    }

