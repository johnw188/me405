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

/// These defines prevent this file from being included more than once in a *.cc file
#ifndef _NRF24L01_BASE_H_
#define _NRF24L01_BASE_H_

#include "avr_queue.h"                      // Template header for circular buffer
#include "spi_bb.h"                         // Header for bit-banged SPI port
#include "base_text_serial.h"               // Header for base serial devices


#define nRF24_MAX_PKT_SZ    32              // Maximum packet size for the radio
#define nRF24_SPI_TIMEOUT   1000            // Retries until timeout for SPI port

#define nRF24_TRANSMIT      0x00
#define nRF24_RECEIVE       0x01

//Define interrupt types for use in virtual interrupt handler
#define nRF24_INT_RX        0x40      // Mask (stop) RX data arrived interrupt
#define nRF24_INT_TX        0x20      // Mask data successfully send interrupt
#define nRF24_INT_M_RT      0x10      // Mask send-has-failed interrupt

//Define interrupt ON/ OFF (interrupt reflected on IRQ pin)
#define nRF24_IRQ_ON        1
#define nRF24_IRQ_OFF       0

//Define CRC ON/OFF
#define nRF24_CRC_ON        0x08
#define nRF24_CRC_OFF       0x00
#define nRF24_CRC_1_BYTE    0x00 
#define nRF24_CRC_2_BYTE    0x04

//Define auto_ack on/off
#define nRF24_A_ACK_ON      0x3f
#define nRF24_A_ACK_OFF     0x00

//Define address width.
#define nRF24_AW_3          0x01
#define nRF24_AW_4          0x02
#define nRF24_AW_5          0x03

//Define data rate and power output
#define nRF24_DR_1M         0x01
#define nRF24_DR_2M         0x09
#define nRF24_RF_POW_0      0x07
#define nRF24_RF_POW_N6     0x05
#define nRF24_RF_POW_N12    0x03
#define nRF24_RF_POW_N18    0x01

//Define data pipes
#define nRF24_PIPE_0        0x01
#define nRF24_PIPE_1        0x02
#define nRF24_PIPE_2        0x04
#define nRF24_PIPE_3        0x08
#define nRF24_PIPE_4        0x10
#define nRF24_PIPE_5        0x20

// These are commands to be sent to the radio
#define nRF24_RD_REG        0x00
#define nRF24_WR_REG        0x20
#define nRF24_RD_PLD        0x61
#define nRF24_WR_PLD        0xa0
#define nRF24_FLUSH_TX      0xe1
#define nRF24_FLUSH_RX      0xe2
#define nRF24_REUSE_TX_PLD  0xe3
#define nRF24_NOP           0xff

//REG_ADDR - Address of registers to send commands to.
#define nRF24_REG_CONF        0x00    // Configuration register
#define nRF24_REG_EN_AA       0x01    // Enable Auto-Ack
#define nRF24_REG_EN_RXADDR   0x02    // Enable RX addresses
#define nRF24_REG_SETUP_AW    0x03    // Address widths ( 3 - 5 bytes)
#define nRF24_REG_SETUP_RETR  0x04    // Setup retransmit
#define nRF24_REG_RF_CH       0x05    // RF Channel (Probably shouldn't touch.)
#define nRF24_REG_RF_SETUP    0x06    // RF Setup (Also probably shouldn't touch...)
#define nRF24_REG_STATUS      0x07    // Status register (First byte out of MISO 
                                      // as MOSI comes)
#define nRF24_REG_OBS_TX      0x08    // Observe transmission. Read only.
#define nRF24_REG_CD          0x09    // Carrier Detect. Leave alone??
#define nRF24_REG_RX_ADDR_P0  0x0A    // Address Pipe 0.
#define nRF24_REG_RX_ADDR_P1  0x0B
#define nRF24_REG_RX_ADDR_P2  0x0C
#define nRF24_REG_RX_ADDR_P3  0x0D
#define nRF24_REG_RX_ADDR_P4  0x0E
#define nRF24_REG_RX_ADDR_P5  0x0F
#define nRF24_REG_TX_ADDR     0x10    // Primary Transmitter address
#define nRF24_REG_PW_P0       0x11    // Data size in FIFO register for Pipe 0
                                      // = How many bytes are in packet received
#define nRF24_REG_PW_P1       0x12
#define nRF24_REG_PW_P2       0x13
#define nRF24_REG_PW_P3       0x14
#define nRF24_REG_PW_P4       0x15
#define nRF24_REG_PW_P5       0x16
#define nRF24_REG_FIFO_STATUS 0x17    // Status register (shifted to MISO as things 
                                      // read out to MOSI)
// Bits in the status register
#define nRF24_RX_DR         0x40
#define nRF24_TX_DS         0x20
#define nRF24_MAX_RT        0x10
#define nRF24_RX_P_NO       0x0e
#define nRF24_TX_FULL       0x01

// Bits in the configuration register (use RX_DR, TX_DS, MAX_RT above for masking)
#define nRF24_EN_CRC        0x08
#define nRF24_CRCO          0x04
#define nRF24_PWR_UP        0x02
#define nRF24_PRIM_RX       0x01

// Standard configurations for transmit, receive, and owl stretching time
#define nRF24_RECV_MODE     nRF24_EN_CRC | nRF24_PWR_UP | nRF24_PRIM_RX \
                            | nRF24_INT_TX | nRF24_INT_M_RT
#define nRF24_XMIT_MODE     nRF24_EN_CRC | nRF24_PWR_UP | nRF24_INT_TX \
                            | nRF24_INT_M_RT
#define nRF24_SPI_MODE      nRF24_INT_TX | nRF24_INT_M_RT | nRF24_EN_CRC


//-------------------------------------------------------------------------------------
/** This class operates a radio module based on a Nordic nRF24L01 chip in text mode.
 *  As a base class, this class is intended to be extended; its descendents will
 *  implement text-based connections and packet-based connections. 
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

class nRF24L01_base
    {
    // Protected data and methods are accessible from this class and its descendents
    protected:
        /// This is the I/O port to which the Chip Enable (CE) line is connected
        volatile unsigned char* port_CE;

        /// This bitmask has a 1 for the Chip Enable (CE) line
        unsigned char mask_CE;

        /// This is the I/O port to which the interrupt request (IRQ) line is connected
        volatile unsigned char* port_IRQ;

        /// This bitmask is used when checking the IRQ signal by polling instead of
        /// using interrupts
        unsigned char mask_IRQ;

        /// This is a pointer to a bit-banged SPI port object
        spi_bb_port* p_spi;

        /// This is a bitmask for the Slave Select (SS on CPU, CSN on the radio) bit
        unsigned char slave_msk;

        /// This is a pointer to a serial port object which is used for debugging the
        /// radio modem code. Left blank, it defaults to NULL and no debugging info
        base_text_serial* p_serport;

    // Public methods can be called from anywhere in the program where there is a 
    // pointer or reference to an object of this class
    public:
        // The constructor sets up the radio interface
        nRF24L01_base (volatile unsigned char&, volatile unsigned char&, unsigned char,
            volatile unsigned char&, volatile unsigned char&, unsigned char, 
            spi_bb_port*, unsigned char slave_mask, base_text_serial* = NULL);

        bool ready_to_send (void);          // Check if the port is ready to transmit
        void reset (void);                  // Reset radio module to starting state

        // Method which sets the address for the transmitter
        // FIXME:  This should be changed to a four-byte address passed in a long int
        void set_TX_address (unsigned char*);

        // Method which sets the address for one pipe in the receiver
        // FIXME:  This should be changed to a four-byte address passed in a long int
        void set_RX_address (unsigned char*, unsigned char = 0);

        // Method to display the contents of the radio chip's registers
        void dump_regs (base_text_serial*, ser_manipulator = bin);

        // Set the width of the payload which the receiver will expect
        void set_payload_width (unsigned char, unsigned char = 0);

        // Put the radio into a standard transmission mode
        void set_transmit_mode (void);
        
        // Put the radio into a standard receiving mode
        void set_receive_mode (void);

        // Method to transmit a packet of data via the radio
        bool transmit (unsigned char*);

        // This method checks if there is a packet of data available
        bool data_ready (void);
    };

#endif  // _NRF24L01_BASE_H_
