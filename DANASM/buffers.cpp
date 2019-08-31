#include "buffers.h"



buffers::buffers()
{
}


buffers::~buffers()
{
}

void buffers::create(int size, byte type) {
	buffer = new byte[size];
	this->type = type;
	this->size = size;
	exist = true;
}

void buffers::destroy() {
	delete[] buffer;
	exist = false;
}

byte buffers::peek(int offset, byte type) {
	return buffer[offset];
}

void buffers::poke(int offset, byte type, byte value) {
	if (offset >= 0 && offset < size) {
		buffer[offset] = value;
	}
}

void buffers::fill(byte value) {
	for (int i = 0; i < size; i++) {
		buffer[i] = value;
	}
}