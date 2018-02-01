/*
  File: MemoryFree.h
  Author: Chris Claxton 2018
  Refer to cpp file for info
*/

#ifndef MemoryInfo_h
  #define MemoryInfo_h
   
  namespace MemoryInfo {
      unsigned int dataStart();
      unsigned int dataSize();
      unsigned int heapStart();
      unsigned int heapEnd();
      unsigned int heapSize();
      unsigned int stackPointer();
      unsigned int stackPointer2(); // alternative method
      unsigned int ramEnd();
      unsigned int stackSize();
      unsigned int stackAvailable() ;
      unsigned int heapAvailable();
      unsigned int heapFree();
      void printAddress(unsigned int val, char (&out)[5]);
  }
   
#endif