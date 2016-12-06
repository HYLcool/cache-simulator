#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <string.h>

#include "def.h"
#include "cache.h"
#include "memory.h"

using namespace std;

enum Mode
{
	A, B, C
};

void ShowUsage();
void testA(fstream & fin);
void testB(fstream & fin);
void testC(fstream & fin ,int back,int allocate);

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
