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
	int size = cachetable -> getOffset_num();

	
	//Caculate the value of tag, setnum, offset;
	int a = int(log(config_.set_num) / log(2) + 0.5);
	int b = int(log(size) / log(2) + 0.5);
	tag = addr >> (a + b);
	setnum = (addr & ((1 << (a + b)) - 1)) >> b;
	//printf("setnum = %d, a = %d, b = %d\n", setnum, a, b);
	offset = addr & ((1 << b) - 1);  
	
	if(offset + bytes > size)
	{
		int tmpb = size - offset;
		int uselesshit, uselesscycle;
		HandleRequest(addr, tmpb, read, content, hit, cycle);
		addr &= (-1) << b;
		addr += size;
		HandleRequest(addr, bytes - tmpb, read, content + tmpb, uselesshit, uselesscycle);
		return;
	}
	lrucount_++;
	stats_.access_counter += 1;
  // Decide on whether a bypass should take place.
  if (!BypassDecision()) {

    // Generate some infomation for the data partition.
    PartitionAlgorithm();

    // Check whether the required data already exist.
    //printf("stat1\n");
    if (ReplaceDecision(tag, setnum, hitline, isfull)) {
    	//printf("stat2\n");
    	if(read == READ)
    	{
    		for (int i = 0; i < bytes; i++) {
    			content[i] = cachetable -> getData(setnum, hitline, offset + i);
    		}
    		//printf("hit!!\n");
    	}
    	else if(read == WRITE)
    	{
    		if(config_.write_through)
    		{
    			stats_.fetch_num += 1;
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
    	stats_.access_cycle += cycle;
    	cachetable -> setLRU(setnum, hitline, lrucount_);
     	
      	return;
    }

    // Choose a victim for the current request.

    //ReplaceAlgorithm();
    stats_.miss_num += 1;
	int index;
	ReplaceAlgorithm(setnum, isfull, index); // handle dirty
	char * buffer = new char[size];
	if(read == READ)
	{
		addr &= (-1) << b;
		stats_.fetch_num += 1;
		if (isfull)
			stats_.replace_num += 1;
		lower_->HandleRequest(addr, size, read, buffer,
			lower_hit, lower_cycle);
		for (int i = 0; i < size; i++) {
			cachetable -> setData(setnum, index, i, buffer[i]);
		}
		cachetable -> setValid(setnum, index, true);
		//printf("setnum = %d, index = %d, Valid = %d\n", setnum, index, (int)cachetable->getValid(setnum, index));
		cachetable -> setTag(setnum, index, tag);
		cachetable -> setDirty(setnum, index, false);
		cachetable -> setLRU(setnum, index, lrucount_);

		for (int i = 0; i < bytes; i++) {
    		content[i] = cachetable -> getData(setnum, index, offset + i);
    	}
    	//printf("not hit!!\n");
    	cycle = latency_.bus_latency + lower_cycle + latency_.hit_latency;
	}
	else if(read == WRITE)
	{		
		if(config_.write_allocate)
		{
			addr &= (-1) << b;
			stats_.fetch_num += 1;
			if (isfull)
				stats_.replace_num += 1;
			lower_->HandleRequest(addr, size, READ, buffer,
				lower_hit, lower_cycle);
			for (int i = 0; i < size; i++)
				cachetable -> setData(setnum, index, i, buffer[i]);
			cachetable -> setValid(setnum, index, true);
			cachetable -> setTag(setnum, index, tag);
			cachetable -> setDirty(setnum, index, false);
			if(config_.write_through)
			{
				printf("You should combine the no write allocate policy with write through!\n");
				exit(1);
			}
			else
			{
				for (int i = 0; i < bytes; i++) {
					cachetable -> setData(setnum, index, offset + i, content[i]);
				}
			}
			cachetable -> setLRU(setnum, index, lrucount_);
			cycle = 0;
		}
		else 
		{
			stats_.fetch_num += 1;
			lower_ -> HandleRequest(addr, bytes, read, content, lower_hit, lower_cycle);
			cycle = lower_cycle;
		}
		cycle += latency_.bus_latency + latency_.hit_latency;
	}
    hit = 0;
    stats_.access_cycle += latency_.bus_latency + latency_.hit_latency;
    

    // Decide on whether a prefetch should take place.
    if (PrefetchDecision()) {
      // Fetch the other data via HandleRequest() recursively.
      // To distinguish from the normal requests,
      // the 2|3 is employed for prefetched write|read data
      // while the 0|1 for normal ones.
      PrefetchAlgorithm();
    }
    delete [] buffer;
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

int Cache::ReplaceDecision(LLU tag, unsigned int set, int & hitline, bool& p) {
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

void Cache::ReplaceAlgorithm(int set, bool isfull, int &index){
	int tlru = 0xfffffff;
	for (int i = 0; i < cachetable -> getLine_num(); i++) 
	{
		if (cachetable -> getValid(set, i) == false && isfull == false) 
		{
			index = i;
			//printf("while not full: index = %d\n", index);
			return;
		}
		if (cachetable -> getLRU(set, i) < tlru)
		{
			tlru = cachetable -> getLRU(set, i);
			index = i;
		}
	}
	//printf("while full: index = %d\n", index);

	if(cachetable -> getDirty(set, index) == true)
	{
		int lower_hit, lower_cycle;
		int size = cachetable -> getOffset_num();
		char * w_buffer = new char[size];
		for(int i = 0; i < size; i++)
		{
			w_buffer[i] = cachetable -> getData(set, index, i);
		}
		uint64_t addr = 0;
		uint64_t ttag = cachetable ->getTag(set, index);
		int a = int(log(config_.set_num) / log(2) + 0.5);
		int b = int(log(cachetable -> getOffset_num()) / log(2) + 0.5);

		addr = ttag << (a + b);
		addr += set << (b);

		lower_->HandleRequest(addr, size, WRITE, w_buffer, lower_hit, lower_cycle);
		delete[] w_buffer;
	}
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