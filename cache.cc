#include "cache.h"
#include "def.h"

// Main access process
// [in]  addr: access address
// [in]  bytes: target number of bytes
// [in]  read: 0|1 for write|read
//             3|4 for write|read in prefetch
// [i|o] content: in|out data
// [out] hit: 0|1 for miss|hit
// [out] cycle: total access cycle
void Cache::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &cycle) {

	uint64_t tag = 0;
	unsigned int setnum = 0;
	unsigned int offset = 0;
	bool isfull = 0;
	int lower_hit, lower_cycle;
	int hitline;

	{
		//Caculate the value of tag, setnum, offset;
		int a = int(log(config_.set_num) / log(2) + 0.5);
		int b = int(log(cachetable -> getOffset_num()) / log(2) + 0.5);
		tag = addr >> (a + b);
		setnum = (addr & ((1 << a + b) - 1)) >> b;
		offset = addr & ((1 << b) - 1);  
	};

  // Decide on whether a bypass should take place.
  if (!BypassDecision()) {

    // Generate some infomation for the data partition.
    PartitionAlgorithm();

    // Check whether the required data already exist.
    if (ReplaceDecision(tag, setnum, hitline, isfull)) {
    	if(read == READ)
    	{
    		for (int i = 0; i < bytes; i++) {
    			content[i] = cachetable -> getData(setnum, hitline, offset + i);
    		}
    	}
    	else if(read == WRITE)
    	{
    		if(config_.write_through)
    		{
    			lower_->HandleRequest(addr, bytes, read, content, lower_hit, lower_cycle);
    			for (int i = 0; i < bytes; i++) {
    				cachetable -> setData(setnum, hitline, offset + i, content[i]);
    			}
    		}
    		else 
    		{
    			for (int i = 0; i < bytes; i++) {
    				cachetable -> setData(setnum, hitline, offset + i, content[i]);
    			}
    			cachetable -> setDirty(setnum, hitline, true);
    		}
    	}
      // Hit!
      // Something else should be done here
      // according to your replacement algorithm.

    	hit = 1;
    	cycle = latency_.bus_latency + latency_.hit_latency;
     
      	return;
    }

    // Choose a victim for the current request.

    //ReplaceAlgorithm();

	int index = 0;
	// ReplaceAlgorithm(isfull, index); // handle dirty
	int size = cachetable -> getOffset_num();
	char * buffer = new char[size];
	if(read == READ)
	{
		lower_->HandleRequest(addr, size, read, buffer,
			lower_hit, lower_cycle);
		for (int i = 0; i < size; i++) {
			cachetable -> setData(setnum, index, i, buffer[i]);
		}
		cachetable -> setValid(setnum, index, true);
		cachetable -> setTag(setnum, index, tag);
		cachetable -> setDirty(setnum, index, false);
	}
	else if(read == WRITE)
	{
		if(config_.write_allocate)
		{
			lower_->HandleRequest(addr, size, READ, buffer,
				lower_hit, lower_cycle);
			for (int i = 0; i < size; i++)
				cachetable -> setData(setnum, index, i, buffer[i]);
			cachetable -> setValid(setnum, index, true);
			cachetable -> setTag(setnum, index, tag);
			cachetable -> setDirty(setnum, index, false);
			if(config_.write_through)
			{
				printf("HEHE! You should combine the write allocate policy with write through!\n");
				exit(1);
			}
			else
			{
				for (int i = 0; i < bytes; i++) {
					cachetable -> setData(setnum, index, offset + i, content[i]);
				}
			}
		}
		else 
		{
			lower_->HandleRequest(addr, bytes, read, content, lower_hit, lower_cycle);
		}
	}
    hit = 0;
    cycle = latency_.bus_latency + lower_cycle;

    // Decide on whether a prefetch should take place.
    if (PrefetchDecision()) {
      // Fetch the other data via HandleRequest() recursively.
      // To distinguish from the normal requests,
      // the 2|3 is employed for prefetched write|read data
      // while the 0|1 for normal ones.
      PrefetchAlgorithm();
    }
  }
  // Fetch from the lower layer

  // If a victim is selected, replace it.
  // Something else should be done here
  // according to your replacement algorithm.
}

int Cache::BypassDecision() {
  return FALSE;
}

void Cache::PartitionAlgorithm() {
}

int Cache::ReplaceDecision(int tag, unsigned int set, int & hitline, bool& p) {
	p = true;
	for (int i = 0; i < cachetable -> getLine_num(); i++) {
		if (cachetable -> getValid(set, i) == TRUE) {
			if (cachetable -> getTag(set, i) == tag) {
				// HIT!!!!
				hitline = i;
				return true;
			}
		}
		else 
			p = false;
	}
	return false;
}

void Cache::ReplaceAlgorithm(int &index){
}

int Cache::PrefetchDecision() {
  return FALSE;
}

void Cache::PrefetchAlgorithm() {
}

void Cache::SetConfig(CacheConfig cc) {
	config_ = cc;
	cachetable = new CacheTable(cc.set_num, cc.associativity,
		cc.size / (cc.set_num * cc.associativity));
}