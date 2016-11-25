#include <stdio.h>
#include "CacheTable.h"

CacheTable::CacheTable(){
}
CacheTable::CacheTable(int set_num_,int line_num_,int offset_num_){
	set_num=set_num_;
	line_num=line_num_;
	offset_num=offset_num_;
	int size=offset_num_*set_num_*line_num_;
	data=new char[size];           //to store data
	for(int i=0;i<=size-1;i++){  //initial all zero
		data[i]=0;
	}
	int lines=set_num_*line_num_;
	valid=new bool[lines];  //the valid
	tag=new int[lines];  //new tag
	dirty = new bool[lines];
	lru = new int[lines];
	for(int i=0;i<=lines-1;i++){         //initial to all zero
		valid[i]=0;
		tag[i]=0;
		dirty[i] = 0;
		lru[i] = 0;
	}
	
}
CacheTable::~CacheTable(){
	delete data;
	delete valid;
	delete tag;
}
bool CacheTable::getValid(int set,int line){
	if(set<0||set>=set_num){
		printf("set num over when get valid\n");
		return -1;
	}
	if(line<0||line>=line_num){
		printf("line num over when get valid\n");
		return -1;
	}
	int num=set*line_num+line;
	return valid[num];


}
int CacheTable::getTag(int set,int line){
	if(set<0||set>=set_num){
		printf("set num over when get tag\n");
		return -1;
	}
	if(line<0||line>=line_num){
		printf("line num over when get tag\n");
		return -1;
	}
	int num=set*line_num+line;
	return tag[num];

}
bool CacheTable::getDirty(int set,int line){
	if(set<0||set>=set_num){
		printf("set num over when get dirty\n");
		return -1;
	}
	if(line<0||line>=line_num){
		printf("line num over when get dirty\n");
		return -1;
	}
	int num=set*line_num+line;
	return dirty[num];

}
int CacheTable::getLRU(int set,int line){
	if(set<0||set>=set_num){
		printf("set num over when get least using time\n");
		return -1;
	}
	if(line<0||line>=line_num){
		printf("line num over when get least using time\n");
		return -1;
	}
	int num=set*line_num+line;
	return lru[num];

}
char CacheTable::getData(int set,int line,int offset){
	if(set<0||set>=set_num){
		printf("set num over when get data\n");
		return -1;
	}
	if(line<0||line>=line_num){
		printf("line num over when get data\n");
		return -1;
	}
	if(offset<0||offset>=offset_num){
		printf("offset num over when get data\n");
		return -1;
	}
	int num=(set*line_num+line)*offset_num+offset;
	return data[num];

}
int CacheTable::getSet_num(){
	return set_num;
}
int CacheTable::getLine_num(){
	return line_num;
}
int CacheTable::getOffset_num(){
	return offset_num;
}

//set
bool CacheTable::setValid(int set,int line,bool value){
	if(set<0||set>=set_num){
		printf("set num over when set valid\n");
		return 0;
	}
	if(line<0||line>=line_num){
		printf("line num over when set valid\n");
		return 0;
	}
	int num=set*line_num+line;
	valid[num]=value;
	return 1;


}
bool CacheTable::setTag(int set,int line,int value){
	if(set<0||set>=set_num){
		printf("set num over when set tag\n");
		return 0;
	}
	if(line<0||line>=line_num){
		printf("line num over when set tag\n");
		return 0;
	}
	int num=set*line_num+line;
	tag[num]=value;
	return 1;

}
bool CacheTable::setDirty(int set,int line,bool value){
	if(set<0||set>=set_num){
		printf("set num over when set dirty\n");
		return 0;
	}
	if(line<0||line>=line_num){
		printf("line num over when set dirty\n");
		return 0;
	}
	int num=set*line_num+line;
	dirty[num]=value;
	return 1;

}
bool CacheTable::setLRU(int set,int line,int value){
	if(set<0||set>=set_num){
		printf("set num over when set least using time\n");
		return 0;
	}
	if(line<0||line>=line_num){
		printf("line num over when set least using time\n");
		return 0;
	}
	int num=set*line_num+line;
	lru[num]=value;
	return 1;

}
bool CacheTable::setData(int set,int line,int offset,char value){
	if(set<0||set>=set_num){
		printf("set num over when set data\n");
		return 0;
	}
	if(line<0||line>=line_num){
		printf("line num over when set data\n");
		return 0;
	}
	if(offset<0||offset>=offset_num){
		printf("offset num over when set data\n");
		return 0;
	}
	int num=(set*line_num+line)*offset_num+offset;
	data[num]=value;
	return 1;

}

bool CacheTable::setSet_num(int value){
	set_num=value;
	return 1;
}
bool CacheTable::setLine_num(int value){
	line_num=value;
	return 1;
}
bool CacheTable::setOffset_num(int value){
	offset_num=value;
	return 1;
}


