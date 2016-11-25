#ifndef CACHE_CACHE_H_
#define CACHE_CACHE_H_

#include <stdint.h>
#include <cmath>
#include <cstdlib>

#include "storage.h"
#include "CacheTable.h"

typedef struct CacheConfig_ {
  int size;
  int associativity;
  int set_num; // Number of cache sets
  int write_through; // 0|1 for back|through
  int write_allocate; // 0|1 for no-alc|alc
} CacheConfig;

class Cache: public Storage {
 public:
  Cache() {}
  ~Cache() {delete cachetable;}

  // Sets & Gets
  void SetConfig(CacheConfig cc);
  void GetConfig(CacheConfig cc);
  void SetLower(Storage *ll) { lower_ = ll; }
  // Main access process
  void HandleRequest(uint64_t addr, int bytes, int read,
                     char *content, int &hit, int &cycle);

 private:
  // Bypassing
  int BypassDecision();
  // Partitioning
  void PartitionAlgorithm();
  // Replacement
  int ReplaceDecision(int tt, unsigned int tsn,int & hitline, bool& p);
  void ReplaceAlgorithm(int &index);
  // Prefetching
  int PrefetchDecision();
  void PrefetchAlgorithm();

  CacheConfig config_;
  Storage *lower_;
  CacheTable *cachetable;
  DISALLOW_COPY_AND_ASSIGN(Cache);
};

#endif //CACHE_CACHE_H_ 
