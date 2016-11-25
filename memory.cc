#include "memory.h"

void Memory::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &cycle) {
	hit = 1;
	cycle = latency_.bus_latency + latency_.hit_latency;
}

