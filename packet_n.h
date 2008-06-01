//*************************************************************************************
/** \file packet_n.h
 *      This file contains a class for packets which hold data being communicated 
 *      through various serial media such as serial cables and radios. This class is 
 *      designed to be a very simple packet protocol, about the simplest one which is 
 *      expected to be useful. 
 *
 *  Packet structure:
 *      \li Address of recipient  - 1 byte
 *      \li Address of sender     - 1 byte
 *      \li Code for type of data - 1 byte
 *      \li Payload               - N bytes
 *      \li Checksum              - 1 byte
 *
 *      The packet content codes are defined and described in the enumeration called
 *      'pkt_code' in this file. 
 *
 *  Revised:
 *      \li 03-29-08  JRR  Original file
 */
//*************************************************************************************

#include "base_text_serial.h"


/// These defines prevent this file from being included more than once in a *.cc file
#ifndef _PACKET_N_H_
#define _PACKET_N_H_


/// This is the default size for packets, used if another size isn't specified
#define PKT_DEFSZ           16


//-------------------------------------------------------------------------------------
/** This enumeration holds the codes for all the different types of contents which the
 *  simple N-byte packet can hold. Not all these types are currently implemented; 
 *  some are reserved for future use if needed. More codes will hopefully be added in
 *  the future to support other types of data to be sent. 
 */

typedef enum pkt_type {
    PKT_NULL,               ///< Packet with no meaningful contents
    PKT_ACK,                ///< Acknowledges correct receipt of a packet
    PKT_PING,               ///< Packet which says hello, expects reply
    PKT_TRACE,              ///< Packet traces where it has traveled
    PKT_DISC,               ///< Discovery, for drop-in networking
    PKT_ANNC,               ///< Announcement to anyone receiving
    PKT_STRING,             ///< A string of text characters 
    PKT_INT_ARRAY,          ///< An array of short (16 bit) integers
    PKT_LONG_ARRAY,         ///< An array of long (32 bit) integers
    PKT_WXDATA,             ///< Data from a weather measurement station
    PKT_ERROR               ///< Packet code shouldn't ever be used
    };


//-------------------------------------------------------------------------------------
/** This class implements an N-byte packet which can be transmitted, received, tested,
 *  and acknowledged upon receipt. The template mechanism is used to allow adjustment
 *  of the number of bytes in the packet. Note that the packet size is fixed at 
 *  compile time and can't be changed during a program run; this is simpler than 
 *  allowing variable packet lengths, because some communication channels such as
 *  Nordic radios are quite difficult to use with variable sized packets. 
 */

template <unsigned char payload_size = PKT_DEFSZ, class address_type = unsigned char>
class packet_n
    {
    // Private data and methods are accessible only from within this class and 
    // cannot be accessed from outside -- even from descendents of this class
    private:

    // Protected data and methods are accessible from this class and its descendents
    protected:
        /// This is the address of the device to which the packet will be sent. 
        address_type addr_to;

        /// This is the address of the device from which packet is being sent.
        address_type addr_from;

        /// This code shows what type of data is in the packet. 
        pkt_type type;

        /// This array holds the "payload" of data which will be sent or received
        unsigned char payload[payload_size];

        /// This is a simple checksum computed by adding address, code, and data bytes
        unsigned char checksum;

    // Public methods can be called from anywhere in the program where there is a 
    // pointer or reference to an object of this class
    public:
        packet_n (void);                    ///< Default constructor makes empty packet

        /// Constructor which creates a packet and fills its contents all at once
        packet_n (address_type, address_type, pkt_type, void*, 
                  unsigned char bytes = payload_size);

        /// This method sets the packet's destination address
        /// @param to The address to which the packet will be sent
        void set_destination_address (address_type to) { addr_to = to; }

        /// This method returns the packet's destination address
        /// @return The address to which the packet is to be sent
        address_type get_destination_address (void) { return (addr_to); }

        /// This method sets the packet's originating address
        /// @param from The address of the sending computer
        void set_source_address (address_type from) { addr_from = from; }

        /// This method returns the packet's source address
        address_type get_source_address (void) { return (addr_from); }

        /// This method sets the packet's data type
        /// @param new_type The type of packet which will be sent
        void set_type (pkt_type new_type) { type = new_type; }

        /// This method returns the number of bytes in the packet's payload
        /// @return The number of bytes in the payload
        unsigned char get_payload_size (void) { return (payload_size); }

        /// This method fills the packet's contents from the given buffer
        void fill_payload (void* p_buf, unsigned char bytes = payload_size);

        /// This method copies the payload into the given buffer
        void copy_payload (void* p_buf, unsigned char bytes = payload_size);
        
        /// This operator allows access to one byte in the payload buffer
        unsigned char& operator [] (unsigned char i) { return payload[i]; }
    };


//-------------------------------------------------------------------------------------
/** This default constructor creates a blank packet. If this packet is sent without
 *  some content being first inserted, any program which receives it should ignore it.
 *  The payload is not initialized, so it should be assumed to contain garbage. 
 */

template <unsigned char payload_size = PKT_DEFSZ, class address_type = unsigned char>
packet_n::packet_n (void)
    {
    set_source_address (0);                 // The addressing data is zero and null
    set_destination_address (0);
    set_type (PKT_NULL);
    }


//-------------------------------------------------------------------------------------
/** This constructor creates a packet with the given sender and recipient addresses,
 *  data type, and contents. 
 *  @param where_to The address of the destination computer 
 *  @param where_from The address of the computer from which the packet is being sent
 *  @param a_type The type of data 
 *  @param p_data A pointer to an array of data to be written into the payload
 *  @param bytes How many bytes of data are to be put in the packet
 */

template <unsigned char payload_size = PKT_DEFSZ, class address_type = unsigned char>
packet_n::packet_n (address_type where_to, address_type where_from, pkt_type a_type,
    void* p_data, unsigned char bytes = payload_size)
    {
    set_destination_address (where_to);
    set_source_address (where_from);
    set_type (a_type);
    fill_payload (p_data, bytes);
    }


//-------------------------------------------------------------------------------------
/** This method copies a bunch of bytes from the given location into the buffer for 
 *  this packet. 
 *  @param p_data A pointer to the data to be copied into this packet
 *  @param bytes The number of bytes to be copied (default is the number of bytes in
 *      the packet); the following bytes will be set to zero.
 */

template <unsigned char payload_size = PKT_DEFSZ, class address_type = unsigned char>
void packet_n::fill_payload (void* p_data, unsigned char bytes)
    {
    unsigned char index;                    // Counts through elements in the buffer

    // Copy the payload bytes from the given address into the buffer
    for (index = 0; index < bytes; index++)
        payload[index] = *p_data++;

    // If the buffer hasn't been completely filled, pad the rest with zeros
    while (index < payload_size)
        payload[index++] = 0;
    }


//-------------------------------------------------------------------------------------
/** This method copies a bunch of bytes from the buffer for this packet into a buffer
 *  at the given location. 
 *  @param p_data A pointer to the data to be copied into this packet
 *  @param bytes The number of bytes to be copied (default is the number of bytes in
 *      the packet); the following bytes will be set to zero.
 */

template <unsigned char payload_size = PKT_DEFSZ, class address_type = unsigned char>
void packet_n::copy_payload (void* p_data, unsigned char bytes)
    {
    // Copy the payload bytes from the given address into the buffer
    for (unsigned char index = 0; index < bytes; index++)
        payload[index] = *p_data++ = payload[index];
    }


#endif  // _PACKET_N_H_
