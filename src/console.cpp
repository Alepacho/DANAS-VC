#include "console.h"

extern buffers sys_spritemap;
extern uint system_tick;
extern bool is_working;
extern byte program_mode;
extern bool debug_mode;
//extern std::vector<std::string> editor_input;

static drawing     *draw;
static std::string *input;
static s_editor_code *e_code;
static scripting   *script;

std::vector<std::string> console::token;
static byte current_color;
static uint width, height;
static uint offset, loffset; //offset and last offset
static std::string literals = "0123456789abcdefn&osrui";

void (*command_list[])() = {
	console::cmd::cls,
	console::cmd::run,
	console::cmd::debug,
	console::cmd::exit,
	console::cmd::aye,
	console::cmd::help
};

std::string command_names[] = {
	"cls", "run", "debug", "exit", "aye", "help"
};


console::console() {
	
}
console::~console() {
}

void console::init(drawing &_draw, std::string &_input, s_editor_code &_code, scripting &_script) {
	draw = &_draw;
	input = &_input;
	e_code = &_code;
	script = &_script;

	insert("&dDanasm&3/&dVc!        &5v0.0.1&n");
	insert("Made by Danas (2019)&n");
	insert("&ehttps://vk.com/daskak&5&n");
	insert("Type '&3help&5' to see all supported commands.&n&n");

}

void console::clean() {
	token.clear();
	//draw = NULL;
}

void console::phrase(char str) {
	switch (str) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f': {
			byte _col = (byte)literals.find(str);
			current_color = _col;
		} break;
		case 'n': {
			height += msheight + 1;
			width = 0;
		} break;
		case '&': {
			uint _wadd = draw->text(nvec((float)width, (float)(msheight / 2 + height)), "&", current_color);
			width += _wadd;
		} break;
	}
}

void console::render() {
	uint _wadd; //width add

	current_color = _c::white;
	height = 0;
	width = 0;

	for (uint i = 0; i < token.size(); i++) {
		std::string _str = token[i];

		if (_str[0] == '&') {
			phrase(_str[1]);
		} else {
			_wadd = draw->text(nvec((float)width, (float)(msheight / 2 + height)), token[i], current_color);
			width += _wadd;
		}

		//draw->text(nvec(127, 7 + (float)(i * msheight)), std::to_string(i) + ". " + token[i], current_color);
	}

	//draw input text
	draw->text(nvec(0, (float)(msheight / 2 + height)), ">", current_color);
	_wadd = draw->text(nvec(mswidth - 1, (float)(msheight / 2 + height)), *input, current_color);
	if (system_tick % (60 * 3) > (30 * 3)) draw->rectangle(nvec((float)(mswidth + _wadd) - 1, (float)(msheight / 2 + height)),
														   nvec(mswidth - 2, (float)(msheight - 3)), _c::orange);

	//draw->text(nvec(127, 63), "Ticks: " + std::to_string(system_tick), _c::orange);
}

void console::insert(std::string str) {
	std::string _str;

	offset = 0;
	loffset = offset;
	if (str.length() > 0) {
		while (offset < str.length()) {
			if (str[offset] == '&') {
				//add pref text
				_str = str.substr(loffset, (offset - loffset));
				if (_str.size() > 0) token.push_back(_str);	//add text

				_str = str.substr(offset, 2);

				//check literal
				bool _found = false;
				for (uint l = 0; l < literals.length(); l++) {
					if (_str[1] == literals[l]) {
						_found = true;
						break;
					}
				}

				if (_found) {
					token.push_back(_str);	//add token
					//printf("literal: %s\n", _str.c_str());
					offset += 2;
					loffset = offset;
					
				} else {
					//printf("wrong literal\n");
					offset += 1;
					loffset = offset;
				}
			} else offset++;
			
		}

		//add final token
		_str = str.substr(loffset, (str.length() - loffset)); //back string (before &)
		if (_str.size() > 0) token.push_back(_str);
	}

	//delete prev lines if console height is bigger than screen height
	if ((height + msheight * 2) > SCREEN_HEIGHT) {
		uint _offset = 0;
		bool _found = false;
		for (uint i = 0; i < token.size(); i++) {
			//printf("finding in token[%i]: %s\n", i, token[i].c_str());
			if (token[i] == "&n") {
				_offset = i + 1;
				//printf("offset: %i\n", _offset);
				_found = true;
				break;
			}
		}

		if (_found) {
			while (_offset > 0) {
				token.erase(token.begin());
				_offset--;
				//printf("deleting token: %s\n", token[0].c_str());
			}

		}
	}
}

void console::command(std::string str) {
	insert(">" + str + "&n");

	if (str.length() > 0) {
		if (str[0] == '&') {
			phrase(str[1]);
		} else {
			uint _size = sizeof(command_names) / sizeof(command_names[0]);
			bool _found = false;
			for (uint i = 0; i < _size; i++) {
				if (str.substr(0, command_names[i].length()) == command_names[i]) {
					command_list[i]();
					_found = true;
					break;
				}
			}
			if (!_found) {
				insert("&7Unknown command.&5&n");
			}
		}
	}
}

void console::cmd::cls() {
	token.clear();
}

void console::cmd::run() {
	//script->destroy();
	script->create(e_code->text);
	program_mode = mode::execute;
}

void console::cmd::debug() {
	debug_mode = !debug_mode;
	std::string _str = (debug_mode) ? "On" : "Off";
	insert("&dDebug Mode: " + _str + "&5&n");
}

void console::cmd::exit() {
	is_working = false;
}

void console::cmd::aye() {
	printf("\n"); printf("\n"); printf("\n");
	byte _yy = 0;

	for (uint i = 0; i < SMAP_SIZE; i++) {
		byte _cindx = sys_spritemap.peek(i);
		if (_cindx != 0) {
			printf("%i, %i, %i,  ", i % SMAP_WIDTH, i / SMAP_WIDTH, _cindx);
			if (_yy != i / SMAP_WIDTH) {
				_yy = i / SMAP_WIDTH;
				printf("\n");
			}
		}
	}
	printf("\n"); printf("\n"); printf("\n");
}

void console::cmd::help() {
	//
	/*buffer.push_back("\n");
	buffer.push_back("[=================================]");*/
	insert("&3HELP&5 - Show list of all commands.&n");
	insert("&3CLS&5 - Clear screen.&n");
	insert("&3RUN&5 - Run loaded game.&n");
	insert("&3EXIT&5 - Exit from the program.&n");
}