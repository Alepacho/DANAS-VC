#pragma once
#include "defs.h"

class buffers {
private:
	byte *buffer;
	bool exist = false;
public:
	buffers();
	~buffers();
	
	int size = 0;
	byte type = 0;

	void create(int size, byte type = buffer::fixed);
	void destroy();
	byte *get() { return buffer; }
	bool exists() { return exist; }

	void fill(byte value);

	byte peek(int offset, byte type = 0);				//read
	void poke(int offset, byte type, byte value);		//write
};

