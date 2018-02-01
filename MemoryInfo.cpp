/*
  File: MemoryFree.h
  Author: Chris Claxton 2018
*/

#include "MemoryInfo.h"
#include "Arduino.h"
#include <avr/io.h>

// Useful defines found in <avr/io.h>
//#define RAMSTART     (0x100) //First On-Chip SRAM Location
//#define RAMEND       0x8FF   // Last On-Chip SRAM Location
//#define XRAMEND      RAMEND
//#define E2END        0x3FF  // The size of the EEPROM page
//#define E2PAGESIZE   4      // The size of the EEPROM page.
//#define FLASHEND     0x7FFF // The last byte address in the Flash program space. 


extern int *__brkval;
extern char __heap_start;
extern size_t __malloc_margin;

/*
  The memory location of the start of the data section, a fixed location
*/
unsigned int MemoryInfo::dataStart()
{
    return (unsigned int)RAMSTART;
}

/*
  The size of all the static data and globals
*/
unsigned int MemoryInfo::dataSize()
{
    return ((unsigned int)&__heap_start - (unsigned int)RAMSTART);
}


/*
  The memory location of the start of the heap,
  affected by the amount of static data and globals
*/
unsigned int MemoryInfo::heapStart() 
{
    return (unsigned int)&__heap_start;
}

/*
  The size of the heap not taking into account any free space within it
*/
unsigned int MemoryInfo::heapSize() 
{
    return ((__brkval == 0 ? (unsigned int)&__heap_start : (unsigned int)__brkval) - (unsigned int)&__heap_start);
}

/*
  The memory location of the end of the heap,
  depends on current heap size
*/
unsigned int MemoryInfo::heapEnd() 
{
    // if __brkval is zero, the heap hasn't been used yet, so return the heap start location
    return (__brkval == 0 ? (unsigned int)&__heap_start : (unsigned int)__brkval); 
}

/*
  Pushing an item on the stack decrements the stack pointer by the size of the item
  A byte pushed onto the stack is placed in the memory location the Stack Pointer points to
  The stack pointer is decremented by the size of the item that was pushed
  The Arduino stack pointer points to the next empty/available location on the stack.
*/
unsigned int MemoryInfo::stackPointer() 
{
    return (unsigned int)SP; //SP should point to the last item stored on the stack
}

/*
  Alternative method, does not seem to be a reliable method in testing,
  compiler optimisations may be interfering with this method
*/
unsigned int MemoryInfo::stackPointer2() 
{
    byte x; 
    return (unsigned int)&x; //as x should be placed on the stack, the address of x indicates the stack pointer position
}

/*
  The size of the stack, affected by function calls etc.
*/
unsigned int MemoryInfo::stackSize()
{
    return ((unsigned int)RAMEND - (unsigned int)SP);
}

/*
  The last memory location, a fixed location
*/
unsigned int MemoryInfo::ramEnd()
{
    return (unsigned int)RAMEND;
}

/*
 reports the space between the heap and the stack.
 It does not report any de-allocated memory that is buried in the heap, buried heap space is not usable by the stack,
 and may be fragmented enough that it is not usable for any heap allocations either.
 The space between the heap and the stack is what you really need to monitor if you are trying to avoid stack crashes.
*/
unsigned int MemoryInfo::stackAvailable() 
{
    return ((unsigned int)SP - (__brkval == 0 ? (unsigned int)&__heap_start : (unsigned int)__brkval)); 
}

// The free list structure as maintained by the avr-libc memory allocation routines.
struct __freelist
{
  size_t sz;
  struct __freelist *nx;
};

/*
  reports the free space buried within the heap (de-allocated memory that is buried within the heap)
  plus the free space between the stack and the heap which the heap can grow into
  minus the __malloc_margin which is 128 bytes to leave a minimum space for the stack.
*/
extern struct __freelist *__flp;
unsigned int MemoryInfo::heapAvailable()
{
    unsigned int stackAvail = ((unsigned int)SP - (__brkval == 0 ? (unsigned int)&__heap_start : (unsigned int)__brkval));
    struct __freelist* current;
    
    // add any freespace found in the freelist
    for (current = __flp; current; current = current->nx)
    {
        stackAvail += 2; /* Add two bytes for the memory block's header  */
        stackAvail += (unsigned int) current->sz;
    }

    return stackAvail - __malloc_margin + 2; 
    // take away the __malloc_margin (reserved space for the stack)
    // add an offset to account for the stack space used by this function
}

/*
  reports the free space buried within the heap (de-allocated memory that is buried within the heap)
*/
unsigned int MemoryInfo::heapFree()
{
    struct __freelist* current;
    unsigned int free = 0;
    
    for (current = __flp; current; current = current->nx)
    {
        free += 2; /* Add two bytes for the memory block's header  */
        free += (unsigned int) current->sz;
    }

    return free;
}

/*
  Prints a formatted memory address
*/
void MemoryInfo::printAddress(unsigned int val, char (&out)[5])
{
    // 2 byte hex value is 4 characters plus NULL terminator
    if (val <= 0xFFFF)
    {
        sprintf(out, "%04x", val);
    }
}