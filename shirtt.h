#ifndef SHIRTT_H_
#define SHIRTT_H_

#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
                                   guaranteed safe to access without
                                   faulting */
                                   
void set_priority(int priority);
void stack_prefault(void);
void stack_setup();
void setup_rt_task(int priority);

#endif
