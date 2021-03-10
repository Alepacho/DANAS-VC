#pragma once
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <vector>
//#include "buffers.h"

//extern uint system_tick;

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned int color;

//Resolution
const static uint SCREEN_WIDTH  = 256;  //192
const static uint SCREEN_HEIGHT = 144; //128
//
const static uint SMAP_WIDTH  = 192;
const static uint SMAP_HEIGHT = 192;

const static uint VRAM_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;			//192 * 128 pixels
const static uint CODE_SIZE = 256 * 256;							//65536 symbols
const static uint SMAP_SIZE = SMAP_WIDTH * SMAP_HEIGHT;				//192x192 sprite map
const static uint FREE_MEM = 1024 * 8;								//8KB
static uint MEMORY_SIZE;

enum mode {
	execute,
	terminal,
	edit
};

namespace buffer {
	enum type {
		fixed,
		grow
	};
}

struct palette {
	color *_color;
	int count;
};

inline int clamp(int value, int min, int max) {
	return std::max(min, std::min(value, max));
}

inline float clamp(float value, float min, float max) {
	return std::max(min, std::min(value, max));
}

/*function clamp(_value, _min, _max)
	return max(_min, min(_value, _max))
end*/

/*enum _c {
	black,	//0
	blue,	//1
	purple,	//2
	green,	//3
	brown,	//4
	dkgray,	//5
	gray,	//6
	white,	//7
	red,	//8
	orange,	//9
	yellow,	//10
	lime,	//11
	cyan,	//12
	violet,	//13
	pink,	//14
	peach	//15
};*/

//https://lospec.com/palette-list/bubblegum-16
enum _c {
	black,	//0
	dkred,	//1
	red,	//2
	orange,	//3
	yellow,	//4
	white,	//5
	ltpink,	//6
	pink,	//7
	purple,	//8
	violet,	//9
	dkcyan,	//10 a
	cyan,	//11 b
	lime,	//12 c
	green,	//13 d
	blue,	//14 e
	dkblue	//15 f
};

struct vec {
	float x, y, z, w;
};

//symbols !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
const static std::string symbol = " abcdefghijklmnopqrstuvwxyz1234567890-=[];,./'\\`";
const static std::string shift  = " ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+{}:<>?\"|~";
const static uint symbol_size[] = { //w, h
  //' '   !     "     #     $     %     &    '      (     )     *     +     ,     -     .     /    
	1, 1, 1, 5, 3, 5, 5, 5, 4, 6, 5, 5, 5, 5, 1, 5, 2, 5, 2, 5, 3, 4, 3, 4, 2, 6, 3, 5, 1, 5, 5, 5,
  /*0     1     2     3     4	  5	    6     7     8     9    */
	4, 5, 2, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5,
  //:     ;     <     =     >     ?     @
    1, 5, 2, 5, 3, 5, 3, 5, 3, 5, 4, 5, 4, 5,
  //A     B     C     D     E     F     G     H     I     J     K     L     M     N     O     P     Q     R     S     T     U     V     W     X     Y     Z
	4, 5, 4, 5, 3, 5, 4, 5, 3, 5, 3, 5, 4, 5, 4, 5, 3, 5, 4, 5, 4, 5, 3, 5, 5, 5, 4, 5, 4, 5, 4, 5, 4, 6, 4, 5, 4, 5, 3, 5, 4, 5, 4, 5, 5, 5, 4, 5, 4, 5, 4, 5,
  //[     \\    ]     ^     _     `
	2, 5, 5, 5, 2, 5, 3, 2, 4, 6, 2, 2,
  //a     b     c     d     e     f     g     h     i     j     k     l     m     n     o     p     q     r     s     t     u     v     w     x     y     z
	4, 5, 4, 5, 3, 5, 4, 5, 4, 5, 3, 5, 4, 7, 4, 5, 1, 5, 2, 7, 4, 5, 1, 5, 5, 5, 4, 5, 4, 5, 4, 7, 4, 7, 3, 5, 4, 5, 3, 5, 4, 5, 4, 5, 5, 5, 3, 5, 4, 7, 4, 5,
  //{     |     }     ~
	3, 5, 1, 5, 3, 5, 4, 2
};
const static byte mswidth  = 5; //max symbol width
const static byte msheight = 7; //max symbol height

//static byte fwidth = 4;
//static byte fheight = 5;

struct mouse {
	int x, y;
	bool pressed, pressing, unpressed;

	//check_region(x, y, w, h);
	bool check_region(int _x, int _y, int _w, int _h) {

		if (x > _x && x <= _x + _w) {
			if (y > _y && y <= _y + _h) {
				return true;
			}
		}
		return false;
	}
};

struct keyboard {
	bool copy = false;
	bool paste = false;
	uint last_key;
	bool caps = false;

	//type of copied data
	std::string dtype = "";
};

static vec nvec(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) {
	vec _vec;
	_vec.x = (float)x;
	_vec.y = (float)y;
	_vec.z = (float)z;
	_vec.w = (float)w;
	return _vec;
}

enum section {
	coding,
	spriting,
	mapping,
	sfxmaking,
	musicmaking
};
enum drawmode {
	brush,
	pipette,
	region,
	fill
};

/*static uint tick() {
	uint _delta = system_tick - system_tick_last;
	system_tick_last = system_tick;
	
	
	//if (system_tick > system_tick_last + 1000) {
	//	system_tick_last = system_tick;
	//}
	return _delta;
}*/

struct col {
	byte r, g, b;
};

struct rect {
	float x, y, w, h;
};

//console
static std::string console_input;

struct s_editor_code {
	std::vector<std::string> text;
	uint line	= 0;
	uint col	= 0;
	vec offset	= nvec(0, 0);	//x, y
};

static s_editor_code editor_code;
