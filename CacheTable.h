class CacheTable{
private:
		char *data;
		bool *valid;
		int *tag;
		bool *dirty;
		int *lru;
		int set_num;
		int line_num;
		int offset_num;
public:
	CacheTable();
	CacheTable(int set_num_,int line_num_,int offset_num_);
	~CacheTable();
	bool getValid(int set,int line);//get
	int getTag(int set,int line);
	char getData(int set,int line,int offset);
	bool getDirty(int set,int line);
	int getLRU(int set,int line);
	int getSet_num();
	int getLine_num();
	int getOffset_num();


	bool setValid(int set,int line,bool value);//set
	bool setTag(int set,int line,int value);
	bool setData(int set,int line,int offset,char value);
	bool setSet_num(int value);
	bool setLine_num(int value);
	bool setOffset_num(int value);
	bool setDirty(int set,int line,bool value);
	bool setLRU(int set,int line, int value);


};
