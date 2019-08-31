#pragma once
#include "defs.h"
#include "drawing.h"
#include "scripting.h"

/*
&0123456789abcdef - colors;					| 0-15
&n				  - next line;				| 16
&&				  - draw '&';				| 17
&o				  - bold text;				| 18
&s				  - strikethrough text;		| 19
&r				  - reset text;				| 20
&u				  - underline text;			| 21
&i				  - italic text;			| 22
*/

class console {
private:
	static std::vector<std::string> token;
public:
	console();
	~console();

	void init(drawing &_draw, std::string &_input, s_editor_code &_code, scripting &_script);
	void clean();
	void render();


	static void insert(std::string str);
	void command(std::string str);
	void phrase(char str);

	//
	struct cmd {
		static void cls();
		static void run();
		static void debug();
		static void exit();
		static void aye();
		static void help();
	};
};

