#include "Mainmemory.h"

void Mainmemory::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &cycle) {

	if(read == READ)
  	{
       if(addr > MAX_SIZE + VIRMEM_OFFSET) 
          printf("een\n");
  	  	for(int i = 0; i < bytes; i++)
      	{
      	  	*(content + i) = Mem[addr + i];
     	  }	
  	}	
  	else if(read == WRITE)
  	{
      	for(int i = 0; i < bytes; i++)
     	{
        	Mem[addr + i] = *(content + i);
      } 
    }
	hit = 1;
	cycle = 0;
}

