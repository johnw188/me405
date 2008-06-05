//======================================================================================
/** \file stl_task.cc
 *      This file contains the definition of a task class. This class implements the
 *      tasks in a multitasking system. These tasks are intended to be run in a 
 *      cooperative multitasking framework, but they can also be worked into an 
 *      interrupt framework or run within an RTOS. 
 * 
 *  Usage
 *      The programmer uses this task by deriving a descendent class from it. Then 
 *      s/he creates a run() method in the new class which implements the states. 
 *      In the run() method, there's a big switch statement which chooses which state
 *      code is to run when the run() method is called. Checking the time and deciding
 *      when to run is taken care of by the schedule() method, which only runs the
 *      task's state code when the task's time interval has been reached; if the time
 *      interval is set to 0, the state code is run every time run() is called.  State
 *      transitions are detected and tracked by this class for testing and debugging
 *      purposes. 
 *
 *  Options
 *    \li Serial port debugging can be enabled by defining STL_DEBUG_XXX and
 *        calling the constructor which is given a pointer to a uart object. The
 *        task will then write information about state transitions to the serial
 *        port during operation. This option is to be used during debugging, then
 *        turned off for production code, as serial port writing takes up time
 *        (and of course requires a serial port to be present and connected). 
 *    \li Execution time profiling can be enabled by defining STL_PROFILING.  This
 *        option causes the execution times of the state functions to be measured
 *        and a simple set of performance data to be kept. Performance data can be
 *        written to a serial port at a convenient time, generally after the system
 *        has been run in test for a while. 
 *        (NOTE: This is a future enhancement, not currently usable)
 * 
 *  Revisions
 *    \li  04-21-07  JRR  Original of this file, derived from UCB's TranRun4 and
 *                        simplified greatly for efficient use in AVR processors
 *    \li  05-07-07  JRR  Small bug fixes
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto. 
 */
//======================================================================================

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "stl_debug.h"                      // Definitions for debugging serial port
#include "stl_us_timer.h"                   // Timer measures real time
#include "stl_task.h"                       // The state transition logic header


//--------------------------------------------------------------------------------------
// This is the static member declaration for class stl_task; see class definition in
// the header file for more information on the item(s)

char stl_task::serial_counter = 0;


//--------------------------------------------------------------------------------------
/** This constructor creates a task object. It must be called by the constructor of 
 *  each task which is written by the user. This constructor sets the time between runs
 *  of the run() method (a timestamp with zero time can be used if a task needs to run
 *  via interrupt, or every single time its schedule() method is called). This 
 *  constructor also saves a pointer to the serial port or radio object which will be 
 *  used to dump debugging information, if serial debugging is being used. 
 *  @param time_interval The time between runs of the task's run() method
 *  @param debug_port A pointer to the serial (or radio) port to be used for debugging.
 *      Leave this parameter off for no serial debugging. 
 */

stl_task::stl_task (const time_stamp& time_interval, STL_DEBUG_TYPE* debug_port)
    {
    // Save the time interval between runs of this task
    interval = time_interval;

    // Save a pointer to the debug port object
    dbg_port = debug_port;

    // Give this task its serial number, then increment the serial number counter
    serial_number = serial_counter++;
    STL_DEBUG_PUTS ("Creating task ");
    STL_DEBUG_WRITE (serial_number);
    STL_DEBUG_PUTS ("\r\n");

    // This task begins life in its waiting to run operational state
    op_state = TASK_WAITING;
    save_op_state = TASK_WAITING;

    // The task begins running in state 0, with no transitions unless called for
    current_state = 0;

    // The first time at which to run the task is as soon as reasonable
    next_run_time.set_time (0);

    #ifdef STL_PROFILING
        // Clear the profile data arrays
        clear_prof_data_method ();
    #endif
    }


//--------------------------------------------------------------------------------------
/** This method sets or changes the time interval between runs of this task. 
 *  @param time_interval The time between runs of the task's run() method
 */

void stl_task::set_interval (const time_stamp& time_interval)
    {
    interval = time_interval;
    }


//--------------------------------------------------------------------------------------
/** This method sets the next run time for the task. It should be used if the task
 *  timer's value is adjusted significantly, or if the task timer is started at a time
 *  long after time zero. If this isn't done, the task will run many times in quick
 *  succession, trying to catch up to the correct run time. Note that if this method
 *  is called and sets the next time far in the past, the same problem will occur.
 *  @param next_time A time stamp containing the next time for the task to run
 */

void stl_task::set_next_run_time (const time_stamp& next_time)
    {
    next_run_time = next_time;
    }


//--------------------------------------------------------------------------------------
/** This method is called by the main task loop to try to run the task. If the task is
 *  in the waiting state, it checks to see if it's time to run yet; if it's in the
 *  suspended state, it doesn't. The task shouldn't be in the running state, because
 *  this method is used by the cooperative scheduler, not the pre-emptive one.
 *  @return True if the task's run() function was executed, false if it was not
 */

bool stl_task::schedule (time_stamp& the_time)
    {
    char next_state;                        // State to which a task will transition

    switch (op_state)
        {
        // If the task has been suspended, don't bother trying to run it
        case (TASK_SUSPENDED):
            return (false);

        // If the task needs to run, check if it needs to run now; if so, run it
        case (TASK_WAITING):
            // If it's not time to run the task yet, exit without running it
            if (!(the_time >= next_run_time))
                return (false);

            // If we get here, it is time to run the task; just continue into the
            // task_pending section below, which will cause the task to run right now

        case (TASK_PENDING):
            // Set the state to waiting for the next time interval. If the task needs
            // to run again immediately, run_again_ASAP() will be called within the
            // run() method, causing the state to be set to TASK_PENDING instead
            op_state = TASK_WAITING;
            next_state = run (current_state);       // Call the run() method
            if (next_state != STL_NO_TRANSITION)    // Detect state transition if any
                {                                   // has occurred
                STL_TRACE_PUTCHAR ('T');
                STL_TRACE_WRITE (serial_number);
                STL_TRACE_PUTCHAR (':');
                STL_TRACE_WRITE (current_state);
                STL_TRACE_PUTCHAR ('-');
                STL_TRACE_WRITE (next_state);
                STL_TRACE_PUTS ("\r\n");

                current_state = next_state;         // Go to next state next time
                }
    
            if (op_state == TASK_WAITING)           // Unless task needs to run again
                next_run_time += interval;          // right away, set next run time

            return (true);                          // The task has run this time

        // If the operational state is anything else, there has been a serious error
        default:
            error_stop ("Illegal operational state");
            break;
        };

//     else if (op_state == blocked)           // Blocked tasks are not currently 
//         return (false);                     // implemented

    }


//--------------------------------------------------------------------------------------
/** This is a base method which the user should overload in each descendent of this 
 *  task class. The run method is where all the user-defined action in the task takes
 *  place. It is either called by the task's schedule() method, in the case of 
 *  cooperative multitasking, or by an interrupt handler, in the case of pre-emptive
 *  multitasking.
 *  @param a_state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 */

char stl_task::run (char a_state)
    {
    STL_DEBUG_PUTS ("Base run() method called for task ");
    STL_DEBUG_WRITE (serial_number);
    STL_DEBUG_PUTS ("\r\n");

    return (STL_NO_TRANSITION);
    }


//--------------------------------------------------------------------------------------
/** This method suspends the task so that it won't run again until after somebody calls 
 *  the resume() method. The state of the task before suspension is saved so that it 
 *  can be restored after suspension.
 */

void stl_task::suspend (void)
    {
    save_op_state = op_state;
    op_state = TASK_SUSPENDED;
    }


//--------------------------------------------------------------------------------------
/** This method resumes the task from suspension, so that the task can run again. The
 *  operational state which was saved at the time of suspension is restored. 
 */

void stl_task::resume (void)
    {
    op_state = save_op_state;
    }


//--------------------------------------------------------------------------------------
/** This method clears the profile data arrays. It's called once at startup, and it can
 *  be called later in order to restart the execution time profiling process. Usually
 *  the user should not call this method but instead use the STL_CLEAR_PROF_DATA macro, 
 *  which causes this method to disappear if profiling is deactivated. 
 */

/*void stl_task::clear_prof_data_method (void)
    {
    for (char count = 0; count < num_states; count++)
        {
        num_runs[count] = 0;
        max_entry_runtime[count] = 0;
        sum_entry_runtime[count] = 0;
        max_run_runtime[count] = 0;
        sum_run_runtime[count] = 0;
        max_trans_runtime[count] = 0;
        sum_trans_runtime[count] = 0;
        }
    }*/


//--------------------------------------------------------------------------------------
/** This method changes the initial state in which the task begins to operate. The 
 *  default initial state is state 0. It should only be used before the task begins to
 *  run, as it sets the current and next state variables and will completely mess up
 *  normal state transition operation if called at any other time. 
 */

void stl_task::set_initial_state (char init_state)
    {
    current_state = init_state;
    }


//--------------------------------------------------------------------------------------
/** This method displays a message (if the program was compiled with serial debugging
 *  enabled) and then causes the processor to freeze in an infinite loop. It should be
 *  used if something awful happened and the safest thing to do is to just stop. Only
 *  use this function if there isn't a reasonable way to write an error state which 
 *  handles exceptions in a more useful manner, such as by turning motors and other
 *  possibly dangerous devices off and then halting. 
 *  @param message The text to be displayed before the processor stops working
 */

void stl_task::error_stop (char const* message)
    {
    STL_DEBUG_PUTS("ERROR in task ");
    STL_DEBUG_WRITE (serial_number);
    STL_DEBUG_PUTS (" state ");
    STL_DEBUG_WRITE (current_state);
    STL_DEBUG_PUTS (": ");
    STL_DEBUG_PUTS (message);
    STL_DEBUG_PUTS ("\r\nProcessing stopped.\r\n");

    cli ();                                 // Disable interrupts
    while (1);                              // Bang...you're dead (until reset)
    }


#ifdef STL_PROFILING
//--------------------------------------------------------------------------------------
/** This method prints the results of execution speed profiling to the given serial
 *  port. It's called by the STL_PRINT_PROFILE() macro, which does nothing unless
 *  execution profiling has been turned on by defining STL_PROFILING. 
 *  @param a_port A pointer to an object of class uart which controls a serial port
 */

void stl_task::print_profile_method (avr_uart* a_port)
    {
    a_port->puts ("Execution profiling not yet enabled\r\n");
    }

#endif  // STL_PROFILING
