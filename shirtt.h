//Some Helpfully Implemented Real Time Tasks
//Name credit goes to DieLawn
#ifndef SHIRTT_H_
#define SHIRTT_H_

#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
                                   guaranteed safe to access without
                                   faulting */
/**
 * \brief Sets the priority of the current task
 * 
 * @param	priority	priority of the current task
 */
void set_priority(int priority);

/**
 * \brief Prefaults the stack
 */
void stack_prefault(void);

/**
 * \brief Sets up the stack
 */
void stack_setup();

/**
 * \brief Sets up a real time task
 * 
 * sets up the stack and sets the priority of the current task
 * 
 * @param	priority	priority of the current task
 */
void setup_rt_task(int priority);

#endif
