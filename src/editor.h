#pragma once
#include "defs.h"
#include "buffers.h"
#include "drawing.h"

class editor {
private:
	//code

	//spriting
	byte selected_color    = 1;
	byte selected_region   = 0;
	byte selected_page     = 0;
	byte selected_drawmode = 0;
	vec reg_offset;	//region offset
	byte draw_region[2]; //current and max
public:
	editor();
	~editor();

	byte section = section::coding;

	void init(drawing &_draw, s_editor_code &_input);
	void render();
	void clean();
};

