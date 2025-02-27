#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#define LOCK_MEMORY_SEGMENT(x) __lock_memory_segment(x)
#define UNLOCK_MEMORY_SEGMENT(x) __unlock_memory_segment(x)

void __lock_memory_segment(int x);
void __unlock_memory_segment(int x);

#endif