#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "def.h"
#include "cache.h"
#include "memory.h"

using namespace std;

enum Mode
{
	A, B, S
};

void ShowUsage();

int main(int argc, char ** argv) {
	int cacheSize, blockSize, lineSize, setAss;
	int hitLatency;
	bool Writeback, Writeallocate;

	Mode mode;

	string op;
	LLU address;

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
		} else if (! strcmp(*(argv + i), "-s")) { // customed mode
			mode = S;
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

	while (exeFile >> op >> address) {
		cout << op << " " << address << endl;
	}

	exeFile.close();

	return 0;
}

void ShowUsage() {
	printf("Our Cache Simulator by Huang Yilun, Ye Gen, Xing Xinyu.\n\n");
	printf("Usage:\n\t./sim <-options> [arguments] <Filename>\n");
	printf("options: \n");
	printf("\t-a\tfor test a in lab 3.1\n");
	printf("\t-b\tfor test b in lab 3.1\n");
	printf("\t-s\tcustomed mode for single-level cache. Need to input more arguments\n");
	printf("\t-u\tprint this usage message\n");
	printf("\narguments:\t<Capacity(kB)> <Associativity> <LineSize(bytes)> <WriteUpdatePolicy> \n");
	printf("\t<Capacity>\t\tthe Capacity of this cache (kB)\n");
	printf("\t<Associativity>\t\tthe number of ways of this cache\n");
	printf("\t<LineSize>\t\tthe line size of this cache (bytes)\n");
	printf("\t<WriteUpdatePolicy>\t0 for write back and 1 for write through\n");

	printf("\n");
}
