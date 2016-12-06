#ifndef CACHE_MEMORY_H_
#define CACHE_MEMORY_H_
#include <iostream>
#include <stdint.h>
#include "storage.h"
#include "def.h"

using namespace std;

#define VIRMEM_OFFSET 0x10000
#define MAX_SIZE 0xF0000

class Mainmemory: public Storage {
 public:
  Mainmemory() {Mem = new char[MAX_SIZE + VIRMEM_OFFSET];}
  ~Mainmemory() {delete[] Mem;}

  // Main access process
  void HandleRequest(uint64_t addr, int bytes, int read,
                     char *content, int &hit, int &cycle);

void MyPrint()
  	{
  		for(int i = 0x10000; i < 0x11000; i++)
  		{
        if (i % 4 == 0)
          cout << endl;
        if (Mem[i] >= 32 && Mem[i] <= 126)
  			cout << Mem[i] << " ";
        else
          cout << (int)(unsigned char)Mem[i] << " ";
  		}
  	}

  	char *getpointtoMem()
  	{
  		return Mem;
  	}
 private:
 	char *Mem;
  // Memory implement

  //DISALLOW_COPY_AND_ASSIGN(Mainmemory);
};

#endif //CACHE_MEMORY_H_ 
