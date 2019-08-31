#pragma once
#include "defs.h"
#include "buffers.h"

class drawing {
	buffers *vram;
	uint vram_size;
public:
	drawing(buffers &_memory, uint size);
	~drawing();

	void point(float x, float y, byte color);
	void line(float x1, float y1, float x2, float y2, byte color);
	void line(vec poss, vec pose, byte color);
	void line(float x1, float y1, float x2, float y2, byte color1, byte color2);
	void triangle(vec pos1, vec pos2, vec pos3, byte color, bool outline = false);
	void circle(vec pos, float radius, byte color, bool outline = false);
	void rectangle(vec pos1, vec pos2, byte color, bool outline = false);
	void fill(byte _color);

	void sprite(vec pos, uint index, byte tcol = 0, bool editor = false);

	uint text(vec pos, std::string str, byte col = _c::white);
};

