CC=g++

all: sim

sim: main.o cache.o memory.o CacheTable.o
	$(CC) -o $@ $^

main.o: cache.h

cache.o: cache.h def.h

memory.o: memory.h

CacheTable.o: CacheTable.h

.PHONY: clean

clean:
	rm -rf sim *.o
