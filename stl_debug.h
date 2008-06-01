//*************************************************************************************
/** \file stl_debug.h
 *      This file contains some headers which define a serial debugging utility for
 *      use with the State-Transition-Logic programming environment. There are several
 *      different devices which can be used to get serial debugging information to a
 *      user, such as serial cables, serial radio modems, and other types of radio
 *      modules. Definitions in this file allow the user to choose a debugging method
 *      or choose not to enable serial debugging at all.
 */

#ifndef _STL_DEBUG_H                        // Prevent this file from being included
#define _STL_DEBUG_H                        // more than once in any compiled file

// Make sure the right serial port (or radio, etc.) header files have been included
#if defined STL_DEBUG_9XSTREAM || defined STL_DEBUG_SERIAL_PORT \
    || defined STL_TRACE_9XSTREAM || defined STL_TRACE_SERIAL_PORT
    #include "base_text_serial.h"
#endif
#if defined STL_DEBUG_9XSTREAM || defined STL_TRACE_9XSTREAM
    #include "base_text_serial.h"
#endif

// This section enables debugging through a regular wired serial port which may be
// connected to a PC through a USB-serial converter or an RS-232 level converter 
#if defined STL_DEBUG_SERIAL_PORT || defined STL_DEBUG_9XSTREAM
    #define STL_SERIAL_DEBUG
    #define STL_DEBUG_TYPE          base_text_serial
#endif // STL_*_SERIAL_PORT

// This section enables state transition tracing through a wired serial port
#if defined STL_TRACE_SERIAL_PORT || defined STL_TRACE_9XSTREAM
    #define STL_SERIAL_TRACE
    #define STL_DEBUG_TYPE          base_text_serial
#endif // STL_TRACE_SERIAL_PORT

// Common macros that are used for all serial debugging methods
#if defined STL_SERIAL_DEBUG
    #define STL_DEBUG_PUTCHAR(x)   if (dbg_port) dbg_port->putchar (x)
    #define STL_DEBUG_PUTS(x)      if (dbg_port) dbg_port->puts (x)
    #define STL_DEBUG_WRITE(x)     if (dbg_port) *dbg_port << (x)
    #define STL_DEBUG_WRITE_HEX(x) if (dbg_port) *dbg_port << hex << (x)
#endif // STL_SERIAL_DEBUG

// Common macros that are used for all serial state transition tracing methods
#if defined STL_SERIAL_TRACE
    #define STL_TRACE_PUTCHAR(x)   if (dbg_port) dbg_port->putchar (x)
    #define STL_TRACE_PUTS(x)      if (dbg_port) dbg_port->puts (x)
    #define STL_TRACE_WRITE(x)     if (dbg_port) *dbg_port << (x)
#endif // STL_SERIAL_DEBUG

// This section shuts off debugging features if none of the above debugging methods
// has been turned on. The pointer to a debugging port object is just a void pointer,
// which acts as a placeholder and will be set to NULL; the functions which write data
// to the debugging port are replaced with nothing
#if !defined STL_DEBUG_SERIAL_PORT && !defined STL_DEBUG_9XSTREAM 
    #define STL_DEBUG_PUTCHAR(x)
    #define STL_DEBUG_PUTS(x)
    #define STL_DEBUG_WRITE(x)
#endif
#if !defined STL_TRACE_SERIAL_PORT && !defined STL_TRACE_9XSTREAM
    #define STL_TRACE_PUTCHAR(x)
    #define STL_TRACE_PUTS(x)
    #define STL_TRACE_WRITE(x)
#endif
#if !defined STL_DEBUG_SERIAL_PORT && !defined STL_DEBUG_9XSTREAM \
    && !defined STL_TRACE_SERIAL_PORT && !defined STL_TRACE_9XSTREAM
    #define STL_DEBUG_TYPE          void
#endif

#endif // _STL_DEBUG_H_ 
