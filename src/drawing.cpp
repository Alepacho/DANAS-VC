#include "drawing.h"
extern buffers sys_spritemap;

drawing::drawing(buffers &_memory, uint size) {
	vram = &_memory;
	vram_size = size;
}

drawing::~drawing()
{
}

void drawing::point(float x, float y, byte color) {
	int _x = (int)x;
	int _y = (int)y;

	//
	if (_x >= 0 && _x < SCREEN_WIDTH && _y >= 0 && _y < SCREEN_HEIGHT) {
		vram->poke(_x + _y * SCREEN_WIDTH, 0, color);
	}
}

void drawing::line(float x1, float y1, float x2, float y2, byte color) {
	//differences 
	float xdiff = x2 - x1;
	float ydiff = y2 - y1;

	//draw point if there's no differences
	if (xdiff == 0 && ydiff == 0) {
		drawing::point(x1, y2, color);
		return;
	}

	//
	float min, max, slope;
	if (abs(xdiff) > abs(ydiff)) {
		if (x1 < x2) {
			min = x1;
			max = x2;
		}
		else {
			min = x2;
			max = x1;
		}

		slope = ydiff / xdiff;
		for (float x = min; x <= max; x++) {
			float y = y1 + (x - x1) * slope;
			drawing::point(x, y, color);
		}
	}
	else {
		if (y1 < y2) {
			min = y1;
			max = y2;
		}
		else {
			min = y2;
			max = y1;
		}

		slope = xdiff / ydiff;
		for (float y = min; y <= max; y++) {
			float x = x1 + (y - y1) * slope;
			drawing::point(x, y, color);
		}
	}
}

void drawing::line(vec pos1, vec pos2, byte color) {
	//differences 
	float xdiff = pos2.x - pos1.x;
	float ydiff = pos2.y - pos1.y;

	//draw point if there's no differences
	if (xdiff == 0 && ydiff == 0) {
		drawing::point(pos1.x, pos1.y, color);
		return;
	}

	//
	float min, max, slope;
	if (abs(xdiff) > abs(ydiff)) {
		if (pos1.x < pos2.x) {
			min = pos1.x;
			max = pos2.x;
		}
		else {
			min = pos2.x;
			max = pos1.x;
		}

		slope = ydiff / xdiff;
		for (float x = min; x <= max; x++) {
			float y = pos1.y + (x - pos1.x) * slope;
			drawing::point(x, y, color);
		}
	}
	else {
		if (pos1.y < pos2.y) {
			min = pos1.y;
			max = pos2.y;
		}
		else {
			min = pos2.y;
			max = pos1.y;
		}

		slope = xdiff / ydiff;
		for (float y = min; y <= max; y++) {
			float x = pos1.x + (y - pos1.y) * slope;
			drawing::point(x, y, color);
		}
	}
}

void drawing::line(float x1, float y1, float x2, float y2, byte color1, byte color2) {
	//differences 
	float xdiff = x2 - x1;
	float ydiff = y2 - y1;

	//draw point if there's no differences
	if (xdiff == 0 && ydiff == 0) {
		drawing::point(x1, y2, color1);
		return;
	}

	//
	float min, max, slope;
	if (abs(xdiff) > abs(ydiff)) {
		if (x1 < x2) {
			min = x1;
			max = x2;
		}
		else {
			min = x2;
			max = x1;
		}

		slope = ydiff / xdiff;
		for (float x = min; x <= max; x++) {
			float y = y1 + (x - x1) * slope;
			byte _color = (byte)(color1 + (color2 - color1) * (x - x1) / xdiff);
			drawing::point(x, y, _color);
		}
	}
	else {
		if (y1 < y2) {
			min = y1;
			max = y2;
		}
		else {
			min = y2;
			max = y1;
		}

		slope = xdiff / ydiff;
		for (float y = min; y <= max; y++) {
			float x = x1 + (y - y1) * slope;
			byte _color = (byte)(color1 + (color2 - color1) * (y - y1) / ydiff);
			drawing::point(x, y, _color);
		}
	}
}

void drawing::circle(vec pos, float radius, byte color, bool outline) {
	float x = floor(abs(radius));
	float y = 0;
	float error = 0;

	while (x >= y) {
		if (outline) {
			drawing::point(pos.x + x, pos.y + y, color);
			drawing::point(pos.x + y, pos.y + x, color);
			drawing::point(pos.x - x, pos.y + y, color);
			drawing::point(pos.x - y, pos.y + x, color);
			drawing::point(pos.x - x, pos.y - y, color);
			drawing::point(pos.x - y, pos.y - x, color);
			drawing::point(pos.x + x, pos.y - y, color);
			drawing::point(pos.x + y, pos.y - x, color);
		}
		else {
			drawing::line(nvec(pos.x - y, pos.y - x), nvec(pos.x + y, pos.y - x), color);
			drawing::line(nvec(pos.x - x, pos.y - y), nvec(pos.x + x, pos.y - y), color);
			drawing::line(nvec(pos.x - x, pos.y + y), nvec(pos.x + x, pos.y + y), color);
			drawing::line(nvec(pos.x - y, pos.y + x), nvec(pos.x + y, pos.y + x), color);
		}

		if (error <= 0.0f) {
			y++;
			error += 2 * y + 1;
		}

		if (error > 0) {
			x--;
			error -= 2 * x + 1;
		}
	}
}

void drawing::triangle(vec pos1, vec pos2, vec pos3, byte color, bool outline) {
	if (pos1.y == pos2.y && pos1.y == pos3.y) return;

	if (outline) {
		drawing::line(pos1, pos2, color);
		drawing::line(pos2, pos3, color);
		drawing::line(pos3, pos1, color);
	}
	else {
		//bubble sort vectors
		if (pos1.y > pos2.y) std::swap(pos1, pos2);
		if (pos1.y > pos3.y) std::swap(pos1, pos3);
		if (pos2.y > pos3.y) std::swap(pos2, pos3);

		float _h = pos3.y - pos1.y;		//total triangle height

		for (float i = 0; i < _h; i++) {
			bool _sh = i > pos2.y - pos1.y || pos2.y == pos1.y;	//second half
			float _shh = _sh ? pos3.y - pos2.y : pos2.y - pos1.y;	//segment height

			float alpha = (i / _h);
			float beta = (i - (_sh ? pos2.y - pos1.y : 0)) / _shh;
			vec A = nvec(pos1.x + (pos3.x - pos1.x) * alpha,
				pos1.y + (pos3.y - pos1.y) * alpha);
			vec B = _sh ? nvec(pos2.x + (pos3.x - pos2.x) * beta,
				pos2.y + (pos3.y - pos2.y) * beta)
				: nvec(pos1.x + (pos2.x - pos1.x) * beta,
					pos1.y + (pos2.y - pos1.y) * beta);

			if (A.x > B.x) std::swap(A, B);
			for (float j = A.x; j <= B.x; j++) {
				drawing::point(j, pos1.y + i, color);
			}
		}
	}
}

void drawing::fill(byte _color) {
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
		drawing::point((float)(i % SCREEN_WIDTH), (float)(i / SCREEN_WIDTH), _color);
	}
}

void drawing::rectangle(vec pos1, vec pos2, byte color, bool outline) {
	//pos1.x - x
	//pos1.y - y
	//pos2.x - w
	//pos2.y - h

	if (outline) {
		drawing::line(nvec(pos1.x		  , pos1.y		   ), nvec(pos1.x + pos2.x, pos1.y		   ), color);
		drawing::line(nvec(pos1.x		  , pos1.y		   ), nvec(pos1.x		  , pos1.y + pos2.y), color);
		drawing::line(nvec(pos1.x         , pos1.y + pos2.y), nvec(pos1.x + pos2.x, pos1.y + pos2.y), color);
		drawing::line(nvec(pos1.x + pos2.x, pos1.y		   ), nvec(pos1.x + pos2.x, pos1.y + pos2.y), color);
	} else {
		for (uint i = 0; i < pos2.y + 1; i++) {
			drawing::line(nvec(pos1.x, pos1.y + i), nvec(pos1.x + pos2.x, pos1.y + i), color);
		}
	}
}

void drawing::sprite(vec pos, uint index, byte tcol, bool editor) {
	
	//position in the sprite map
	//vec _smpos;
	//_smpos.x = (float)(index % SMAP_WIDTH) * 8;
	//_smpos.y = (float)(index / SMAP_WIDTH) * 8;

	buffers *_buf = editor ? &sys_spritemap : vram;

	//192 / 8 = 24; 24 * 24 = 576 sprites
	if (index <= 576) {
		for (int _y = 0; _y < 8; _y++) {
			for (int _x = 0; _x < 8; _x++) {
				vec _lpos;
				_lpos.x = (float)(index % 24) * 8;
				_lpos.y = (float)(index / 24) * 8;
				byte _col = _buf->peek((int)_lpos.x + _x + (_y + (int)_lpos.y) * SMAP_WIDTH);

				if (_col != tcol) drawing::point(pos.x + _x, pos.y + _y, _col);
			}
		}
	}
	
}

uint drawing::text(vec pos, std::string str, byte col) {
	//fwidth
	uint text_width = 0;
	for (int i = 0; i < (int)str.length(); i++) {
		uint _symbol = (uint)str[i] - 32;
		uint index = 24 * 3 + _symbol;

		//
		
		uint _width  = symbol_size[_symbol * 2] + 1;
		uint _height = symbol_size[_symbol * 2 + 1];
		text_width += _width;

		//printf("S: %c, W: %i, H: %i\n", _symbol, _width, _height);

		vec _lpos;
		_lpos.x = (float)(index % 24) * 8;
		_lpos.y = (float)(index / 24) * 8;

		if (index <= 576) {
			for (uint _y = 0; _y <= _height; _y++) {
				for (uint _x = 0; _x <= _width; _x++) {
					
					byte _col = sys_spritemap.peek((int)_lpos.x + _x + (_y + (int)_lpos.y) * SMAP_WIDTH);

					if (_col != _c::black) drawing::point(pos.x + _x, pos.y + _y, col);
				}
			}
		}
		pos.x += _width;
	}

	return text_width;
}