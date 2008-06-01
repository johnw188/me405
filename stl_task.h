//======================================================================================
/** \file stl_task.h
 *    This file contains the definition of the task class. This class implements the
 *    tasks in a multitasking system. These tasks are intended to be run in a 
 *    cooperative multitasking framework, but they can also be worked into an 
 *    interrupt framework or run within an RTOS. 
 *
 *  Note:
 *    See usage information in the comments for stl_task.cc, or the automatically
 *    generated documentation files. 
 *
 *  Revisions:
 *    \li  05-01-07  JRR  Original of this file, derived from UCB's TranRun4 and
 *                        simplified greatly for efficient use in AVR processors
 *    \li  05-07-07  JRR  Small bug fixes
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto. 
 */
//======================================================================================

#ifndef _STL_TASK_H_                        // To prevent task.h from being included
#define _STL_TASK_H_                        // in a source file more than once


//--------------------------------------------------------------------------------------
/** This define specifies a no-transition code which means that the next state will be
 *  the same as the current state, and with no self-transition. 
 */

const char STL_NO_TRANSITION = 0xFF;

/** This macro activates the printing of profile data if profiling is turned on, and
 *  deactivates profiling entirely if it's turned off
 */
#ifdef STL_PROFILING
    #define STL_PRINT_PROFILE(x) print_profile_method(x) 
#else
    #define STL_PRINT_PROFILE(x)
#endif


//--------------------------------------------------------------------------------------
/** This enumeration lists the possible operational states of a task. These states are 
 *  not the same as the states which are programmed by the user; these states are only 
 *  used by the task scheduler to figure out which task to run at a given time 
 *  according to priorities (if used) and which tasks are ready to be run. 
 */

typedef enum task_op_state 
    {
    TASK_RUNNING,           ///< The task's run() function is executing
    TASK_PENDING,           ///< The task needs to run again as soon as possible
    TASK_WAITING,           ///< The task is waiting for its next run time to occur
    TASK_BLOCKED,           ///< The task cannot run because a resource is unavailable
    TASK_SUSPENDED          ///< The task is turned off until its resume() is called
    };


//--------------------------------------------------------------------------------------
/** This class implements the behavior of a task in the context of a multitasking
 *  system. Each task runs "simultaneously" with other tasks. This means, of course,
 *  that execution switches quickly from one task to another quite quickly under the
 *  control of the main execution scheduler. Since a task can be in any of several 
 *  states at a given time, a task object will have a list of state methods which
 *  can be run and be able to switch between those states. 
 */

class stl_task
    {
    private:
        static char serial_counter;         // Counter for automatic serial numbers
        task_op_state op_state;             // Operational state of this task
        task_op_state save_op_state;        // For saving states of suspended tasks
        char serial_number;                 // Each task has a serial number
        char current_state;                 // State in which we're currently running

    protected:
        time_stamp next_run_time;           // Time when task should run next
        time_stamp interval;                // Time interval between runs of the task
        STL_DEBUG_TYPE* dbg_port;           // Port for serial debugging information

    public:
        // The constructor sets time interval between runs and debug port (if used)
        stl_task (const time_stamp&, STL_DEBUG_TYPE* = NULL);

        // This metod sets or changes the time interval between runs of the task
        void set_interval (const time_stamp&);

        // This method sets the next time the task is to run
        void set_next_run_time (const time_stamp&);

        bool schedule (time_stamp&);        // Scheduler calls this to try to run task
        virtual char run (char);            // Base method which the user overloads
        void suspend (void);                // Set operational state to suspended
        void resume (void);                 // Un-suspend a task so it can run again
        void set_initial_state (char);      // Set a new state in which to start up

        /** This method returns the task's automatically assigned serial number. 
         *  @return The task's serial number
         */
        char get_serial_number (void) { return (serial_number); }

        /** This method returns the task's current operational state. The operational
         *  state isn't the same as the state transition logic state; it's a separate
         *  variable which controls if the task is running at a given time. 
         *  @return The task's current operational state 
         */
        task_op_state get_op_state (void) { return (op_state); }

        /** This method will cause the task to run again as soon as it can instead of
         *  waiting for the given time interval. 
         */
        inline void run_again_ASAP (void) { op_state = TASK_PENDING; }
        
        /** This method tells whether the task needs to run again as soon as possible
         *  or not. It is convenient to use when determining if the processor should
         *  be put to sleep for a while.
         *  @return True if the task needs to run soon, false if it does not
         */
        inline bool ready (void) { return (op_state == TASK_PENDING 
            || op_state == TASK_RUNNING); }

        void error_stop (char const*);      // Complain and stop the processor

    #ifdef STL_PROFILING                    // Stuff for execution time profiling
    protected:
        int *num_runs;                      // All these variables are for collecting
        long *max_run_runtime;              // test functions take to run 
        long *sum_run_runtime;
    public:
        void print_profile_method (avr_uart*);  // Display execution time profile data
        void clear_prof_data_method (void);     // Clear profiling data arrays
    #endif  // STL_PROFILING
    };

#endif // _STL_TASK_H_
