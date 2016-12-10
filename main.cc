#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <string.h>
#include <cstdarg>

#include "def.h"
#include "cache.h"
#include "memory.h"

using namespace std;

enum Mode
{
	A, B, C, D32
};

struct Level
{
	int capacity_;
	int associativity_;
	int set_;
	int hit_;
};

int level;
Level linfo[3];

void ShowUsage();
void testA(fstream & fin);
void testB(fstream & fin);
void testC(fstream & fin ,int back,int allocate);
void test321(char * filename);

int main(int argc, char ** argv) {
	int cacheSize, blockSize, lineSize, setAss;
	int hitLatency;
	bool Writeback, Writeallocate;

	Mode mode;
	int back, allocate;

	// parse the options
	// if there is no arguments, print the usage message and return
	if (argc <= 1) {
		ShowUsage();
		return 0;
	}

	for (int i = 1; i < argc - 1; i++) {
		if (!strcmp(*(argv + i), "-a")) { // test a
			mode = A;
		} else if (! strcmp(*(argv + i), "-u")) { // print the usage message
			ShowUsage();
			exit(0);
		} else if (! strcmp(*(argv + i), "-c")) { // customed mode
			mode = C;
			back = atoi(*(argv + i + 1));
			allocate = atoi(*(argv + i + 2));
			i += 2;
		} else if (! strcmp(*(argv + i), "-b")) { // test b
			mode = B;
		} else if (! strcmp(*(argv + i), "-32")) { // test 1 in lab 3.2
			mode = D32;
			i += 1;
			level = atoi(*(argv + i));
			i += 1;
			for (int j = 0; j < level; j++) {
				linfo[j].capacity_ = atoi(*(argv + i));
				i += 1;
				linfo[j].associativity_ = atoi(*(argv + i));
				i += 1;
				linfo[j].set_ = atoi(*(argv + i));
				i += 1;
				linfo[j].hit_ = atoi(*(argv + i));
				i += 1;
			}
		} else { // unknown argument
			printf("Unknown argument : %s\n", *(argv + i));
			exit(1);
		}
	}

	// get the executable file
	char * exeFilename = *(argv + (argc - 1));
	fstream exeFile;
	exeFile.open(exeFilename, ios::in);
	if (!exeFile) {
		printf("The file <%s> is not existed\n", exeFilename);
		exit(1);
	}

	switch (mode) {
		case A:
			testA(exeFile);
			break;
		case B:
			testB(exeFile);
			break;
		case C:
			testC(exeFile, back, allocate);
			break;
		case D32:
			test321(exeFilename);
			break;
		default:
			cout << "WRONG!!!" << endl;
			break;
	}

	exeFile.close();

	return 0;
}

void ShowUsage() {
	printf("Our Cache Simulator by Huang Yilun, Ye Gen, Xing Xinyu.\n\n");
	printf("Usage:\n\t./sim <-options> [arguments] <Filename>\n");
	printf("options: \n");
	printf("\t-a\tfor the first test in test A\n");
	printf("\t-b\tfor the second test in test A\n");
	printf("\t-c\tfor the third test in test A, there are 2 arguments here:\n");
	printf("\t\t\t<back&through> <allocate>: 0 for back and 1 for through, 0 for no allocate and 1 for allocate\n");
	printf("\t-32\tfor the test 1 in lab 3.2, there are plenty of arguments.\n");
	printf("\t\t\t<level> [<capacity> <associativity> <set> <hit cycle>]: <level> is the level number of cache,\n\t\t\twith each level, we should tell the <capacity> ,the <associativity>, the <set> number and the <hit cycle>.\n");
	printf("\t-u\tprint this usage message\n");
	printf("\n");
}

void testA(fstream & fin) {
	int size = 32768;
	for(int i = 0; i < 11 ;i++)
	{
		for (int j = 8; j <= 1024; j *= 2 ) {
			string op;
			LLU address;

			char * buffer = new char[2];
			int hit;
			int cycle;
			int thit = 0, tmiss = 0;
			int total = 0;

			Cache * L1 = new Cache();
			Memory * mem = new Memory();

			CacheConfig l1conf;
			l1conf.size = size;
			l1conf.associativity = 1;
			l1conf.set_num = size / (1 * j);
			l1conf.write_through = WRITE_BACK;
			l1conf.write_allocate = WRITE_ALLOCATE;

			StorageLatency sl1;
			sl1.hit_latency = 2;
			sl1.bus_latency = 10;

			StorageLatency mems;
			mems.hit_latency = 100;
			mems.bus_latency = 10;

			L1 -> SetLatency(sl1);
			L1 -> SetConfig(l1conf);
			L1 -> SetLower(mem);
			mem -> SetLatency(mems);

			while (fin >> op >> address) {
				// cout << op << " " << address << endl;
				if (op[0] == 'w') {
					L1 -> HandleRequest(address, 1, WRITE, buffer, hit, cycle);
					//cout << hit << " " << cycle << endl;
					total += cycle;
				} else {
					L1 -> HandleRequest(address, 1, READ, buffer, hit, cycle);
					//cout << hit << " " << cycle << endl;
					total += cycle;
				}
				thit += hit;
				tmiss += (1 - hit);
			}
			// cout << "Case " << i << ":" <<endl;
			//cout << thit << " " << tmiss << endl;			
			cout << tmiss / (thit + tmiss + 0.0) << endl;
			// cout << "Total : " << total << endl;
			fin.clear();
			fin.seekg(0, ios::beg);
			delete [] buffer;
			delete L1;
			delete mem;
		}
		cout << endl;
		size *= 2;
	}
}


void testB(fstream & fin) {
	int size = 32768;
	for(int i = 0; i < 11 ;i++)
	{
		for (int j = 1; j <= 32; j *= 2 ) {
			string op;
			LLU address;

			char * buffer = new char[2];
			int hit;
			int cycle;
			int thit = 0, tmiss = 0;
			int total = 0;

			Cache * L1 = new Cache();
			Memory * mem = new Memory();

			CacheConfig l1conf;
			l1conf.size = size;
			l1conf.associativity = j;
			l1conf.set_num = size / (128 * j);
			l1conf.write_through = WRITE_BACK;
			l1conf.write_allocate = WRITE_ALLOCATE;

			StorageLatency sl1;
			sl1.hit_latency = 2;
			sl1.bus_latency = 10;

			StorageLatency mems;
			mems.hit_latency = 100;
			mems.bus_latency = 10;

			L1 -> SetLatency(sl1);
			L1 -> SetConfig(l1conf);
			L1 -> SetLower(mem);
			mem -> SetLatency(mems);

			while (fin >> op >> address) {
				// cout << op << " " << address << endl;
				if (op[0] == 'w') {
					L1 -> HandleRequest(address, 1, WRITE, buffer, hit, cycle);
					//cout << hit << " " << cycle << endl;
					total += cycle;
				} else {
					L1 -> HandleRequest(address, 1, READ, buffer, hit, cycle);
					//cout << hit << " " << cycle << endl;
					total += cycle;
				}
				thit += hit;
				tmiss += (1 - hit);
			}
			// cout << "Case " << i << ":" <<endl;
			//cout << thit << " " << tmiss << endl;			
			cout << tmiss / (thit + tmiss + 0.0) << endl;
			// cout << "Total : " << total << endl;
			fin.clear();
			fin.seekg(0, ios::beg);
			delete [] buffer;
			delete L1;
			delete mem;
		}
		cout << endl;
		size *= 2;
	}
}

void testC(fstream & fin, int back, int allocate) {
	int size = 32768;
	string op;
	LLU address;

	char * buffer = new char[2];
	int hit;
	int cycle;
	int thit = 0, tmiss = 0;
	int total = 0;

	Cache * L1 = new Cache();
	Memory * mem = new Memory();

	CacheConfig l1conf;
	l1conf.size = size;
	l1conf.associativity = 8;
	l1conf.set_num = 32;
	l1conf.write_through = back;
	l1conf.write_allocate = allocate;

	StorageLatency sl1;
	sl1.hit_latency = 2;
	sl1.bus_latency = 10;

	StorageLatency mems;
	mems.hit_latency = 100;
	mems.bus_latency = 10;

	L1 -> SetLatency(sl1);
	L1 -> SetConfig(l1conf);
	L1 -> SetLower(mem);
	mem -> SetLatency(mems);

	while (fin >> op >> address) {
		// cout << op << " " << address << endl;
		if (op[0] == 'w') {
			L1 -> HandleRequest(address, 1, WRITE, buffer, hit, cycle);
			//cout << hit << " " << cycle << endl;
			total += cycle;
		} else {
			L1 -> HandleRequest(address, 1, READ, buffer, hit, cycle);
			//cout << hit << " " << cycle << endl;
			total += cycle;
		}
		thit += hit;
		tmiss += (1 - hit);
	}
	// cout << "Case " << i << ":" <<endl;
	//cout << thit << " " << tmiss << endl;			
	cout << tmiss / (thit + tmiss + 0.0) << endl;
	cout << "Total : " << total << endl;
	fin.clear();
	fin.seekg(0, ios::beg);
	delete [] buffer;
	delete L1;
	delete mem;
}

// for the first test in lab3.2
void test321(char * filename) {
	// open the file
	FILE * fd = fopen(filename, "r");
	if (fd < 0) {
		printf("Unable to open file : %s\n", filename);
		exit(1);
	}

	char *op = new char[3];
	LLU address;
	char * buffer = new char[2];

	int hit, cycle;
	LLU thit = 0, tmiss = 0, total = 0;
	const int back = WRITE_BACK;
	const int allocate = WRITE_ALLOCATE;

	int bus[3] = {0, 3, 6};

	Memory * mem = new Memory();
	StorageLatency mems;
	mems.hit_latency = 100;
	mems.bus_latency = 0;
	mem -> SetLatency(mems);

	Cache * cachel[3];
	for (int i = 0; i < level; i++) {
		cachel[i] = new Cache();

		CacheConfig config;
		config.size = linfo[i].capacity_;
		config.associativity = linfo[i].associativity_;
		config.set_num = linfo[i].set_;
		config.write_through = back;
		config.write_allocate = allocate;

		StorageLatency cachelatency;
		cachelatency.hit_latency = linfo[i].hit_;
		cachelatency.bus_latency = bus[i];

		cachel[i] -> SetLatency(cachelatency);
		cachel[i] -> SetConfig(config);

		if (i > 0) {
			cachel[i - 1] -> SetLower(cachel[i]);
		}
		if (i == level - 1) {
			cachel[i] -> SetLower(mem);
		}
	}

	int times = 0;
	while (fscanf(fd, "%s %llx", op, &address) == 2) {
		if (op[0] == 'w') {
			cachel[0] -> HandleRequest(address, 1, WRITE, buffer, hit, cycle);
		} else if (op[0] == 'r') {
			cachel[0] -> HandleRequest(address, 1, READ, buffer, hit, cycle);
		}
		total += cycle;
		thit += hit;
		tmiss += (1 - hit);
		times ++;
	}

	// cout << "Miss Rate: " << tmiss * 100.0 / (tmiss + thit + 0.0) << "%" << endl;
	// cout << "Total cycles: " << total << endl;
	// cout << "times: " << times << endl;
	for (int i = 0; i < level; i++) {
		printf("%d,%d,%d,", linfo[i].capacity_, linfo[i].associativity_, linfo[i].set_);
	}
	printf("%lf%%,%llu\n", tmiss * 100.0 / (tmiss + thit + 0.0), total);

	delete [] buffer;
	delete mem;
	for (int i = 0; i < level; i++) {
		delete cachel[i];
	}
	fclose(fd);
}