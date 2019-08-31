#include "scripting.h"

extern byte _menu;
extern uint MEMORY_SIZE;
extern byte program_mode;

static drawing *draw;
static buffers *ram;
std::string str_error;

scripting::scripting() {
}
scripting::~scripting() {
}

void scripting::init(drawing &_draw, buffers &_memory) {
	draw = &_draw;
	ram = &_memory;

	_lua = luaL_newstate();

	load_builtins();

	//clear lua stack
	lua_settop(_lua, 0);

	load_functions();
}


static const std::string func_name[] = {
	"_MAIN", "PEEK", "POKE", "TEXT", "CLS", "PSET", "LINE", "CIRC", "RECT", "TRI"
};

//NULL CUZ WE CHANGE THESE FUNCS INSIDE 
static const lua_CFunction func_list[] = {	//list of all functions
	NULL,
	scripting::PEEK,
	scripting::POKE,
	scripting::TEXT,
	scripting::CLS,
	scripting::PSET,
	scripting::LINE,
	scripting::CIRC,
	scripting::RECT,
	scripting::TRI
};

void scripting::load_builtins() {
	static const luaL_Reg libs[] = {
		{ "_G", luaopen_base },
		{ LUA_LOADLIBNAME, luaopen_package },
		{ LUA_COLIBNAME, luaopen_coroutine },
		{ LUA_TABLIBNAME, luaopen_table },
		{ LUA_STRLIBNAME, luaopen_string },
		{ LUA_MATHLIBNAME, luaopen_math },
		{ LUA_DBLIBNAME, luaopen_debug },
		{ NULL, NULL }
	};

	for (const luaL_Reg *_lib = libs; _lib->func != NULL; _lib++) {
		luaL_requiref(_lua, _lib->name, _lib->func, 1);
		lua_pop(_lua, 1);
	}
}

void scripting::load_functions() {
	int size = sizeof(func_list) / sizeof(lua_CFunction);
	printf("SIZE: %i\n", size);
	for (int i = 0; i < size; i++) {
		if (func_list[i]) {
			lua_pushcfunction(_lua, func_list[i]);
			lua_setglobal(_lua, func_name[i].c_str());
		} else {
			printf("ERROR LOADING FUNC: %s\n", func_name[i].c_str());
		}
	} 
}

void scripting::create(std::vector<std::string> _code) {
	for (uint i = 0; i < _code.size(); i++) {
		code.append(_code[i]);
		printf("%s\n", _code[i].c_str());
		code.append("\n");
	}
	//printf("[[]]\ncode:\n%s[[]]\n", code.c_str());

	lua_settop(_lua, 0);

	//load and run code
	if (luaL_loadstring(_lua, code.c_str()) != LUA_OK || lua_pcall(_lua, 0, LUA_MULTRET, 0) != LUA_OK) {
		printf("%s\n", lua_tostring(_lua, -1));
	}

	//lua_settop(_lua, 0);
}

void scripting::destroy() {
	lua_settop(_lua, 0);
	lua_close(_lua);
}

bool scripting::execute() {
	lua_getglobal(_lua, func_name[0].c_str()); //
	if (lua_isfunction(_lua, -1)) {
		if (lua_pcall(_lua, 0, 0, 0)) {
			std::string _error = lua_tostring(_lua, -1);
			error("[ERROR::LUA] " + _error);
			return 1;
		}
	} else {
		lua_pop(_lua, 1);
		error("[ERROR::LUA] ADD '_MAIN' FUNCTION FIRST");
		return 1;
	}

	return 0;
}

void scripting::error(std::string _text) {
	str_error = _text;
	printf("%s\n", _text.c_str());
	//s_console->insert("&7&"+ _text + "5&n");
}

std::string scripting::get_error() {
	return str_error;
}

//PEEK(index)
//read data
int scripting::PEEK(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc == 1) {
		uint _offset = (uint)lua_tonumber(_lua, 1);

		if (_offset >= 0 && _offset < MEMORY_SIZE) {
			lua_pushinteger(_lua, ram->peek(_offset));
		} //else error("[ERROR::LUA::PEEK] Invalid arguments!");
	} else error("[ERROR::LUA::PEEK] Invalid arguments!");

	return 0;
}

//POKE(index, value)
//write data
int scripting::POKE(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc == 2) {
		uint _offset = (uint)lua_tonumber(_lua, 1);
		byte _value = (byte)lua_tonumber(_lua, 2);

		if (_offset >= 0 && _offset < MEMORY_SIZE) {
			ram->poke(_offset, 0, _value);
		} //else error("[ERROR::LUA::PEEK] Invalid arguments!");
	}
	else error("[ERROR::LUA::POKE] Invalid arguments!");

	return 0;
}

//TEXT(text)
int scripting::TEXT(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc >= 3) {
		if (argc > 4) {
			error("[ERROR::LUA::PRINT] Invalid arguments!");
		}

		vec _pos;
		_pos.x = (float)lua_tonumber(_lua, 1);
		_pos.y = (float)lua_tonumber(_lua, 2);
		std::string _str = lua_tostring(_lua, 3);
		//printf("%s\n", _str.c_str());
		byte _col;
		if (argc == 4) {
			_col = (byte)lua_tonumber(_lua, 4);
		} else {
			_col = _c::white;
		}
		uint _len = draw->text(_pos, _str, _col);
		lua_pushnumber(_lua, _len);
	} else {
		error("[ERROR::LUA::PRINT] Invalid arguments!");
	}
	return 1;
}


//GRAPHICS

//CLS(color)
int scripting::CLS(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc == 1) {
		byte  _color = (byte)lua_tonumber(_lua, 1);
		draw->fill(_color);
	} else error("[ERROR::LUA::FILL] Invalid arguments!");

	return 0;
}

//PSET(x, y, color)
int scripting::PSET(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc == 3) {
		vec _pos;
		_pos.x = (float)lua_tonumber(_lua, 1);
		_pos.y = (float)lua_tonumber(_lua, 2);
		byte  _color = (byte)lua_tonumber(_lua, 3);
		draw->point(_pos.x, _pos.y, _color);
	} else error("[ERROR::LUA::POINT] Invalid arguments!");
	
	return 0;
}

//LINE(x1, y1, x2, y2, color)
int  scripting::LINE(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc == 5) {
		vec _pos1;
		_pos1.x = (float)lua_tonumber(_lua, 1);
		_pos1.y = (float)lua_tonumber(_lua, 2);
		vec _pos2;
		_pos2.x = (float)lua_tonumber(_lua, 3);
		_pos2.y = (float)lua_tonumber(_lua, 4);
		byte  _color = (byte)lua_tonumber(_lua, 5);

		draw->line(_pos1, _pos2, _color);
	} else error("[ERROR::LUA::LINE] Invalid arguments!");

	return 0;
}

//CIRC(x, y, radius, color, [outline])
int scripting::CIRC(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc >= 4) {
		if (argc > 5) {
			error("[ERROR::LUA::CIRCLE] Invalid arguments!");
		}
		vec _pos;
		_pos.x = (float)lua_tonumber(_lua, 1);
		_pos.y = (float)lua_tonumber(_lua, 2);
		float _radius = (float)lua_tonumber(_lua, 3);
		byte  _color = (byte)lua_tonumber(_lua, 4);

		bool _outline = false;
		if (argc >= 5) {
			_outline = lua_toboolean(_lua, 5);
		}

		draw->circle(_pos, _radius, _color, _outline);
	} else error("[ERROR::LUA::CIRCLE] Invalid arguments!");

	return 0;
}

//RECT(x1, y1, x2, y2, color, [outline])
int scripting::RECT(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc >= 5) {
		if (argc > 6) {
			error("[ERROR::LUA::RECT] Invalid arguments!");
		}
		vec _pos1, _pos2;
		_pos1.x = (float)lua_tonumber(_lua, 1);
		_pos1.y = (float)lua_tonumber(_lua, 2);
		_pos2.x = (float)lua_tonumber(_lua, 3);
		_pos2.y = (float)lua_tonumber(_lua, 4);
		byte  _color = (byte)lua_tonumber(_lua, 5);

		bool _outline = false;
		if (argc >= 6) {
			_outline = lua_toboolean(_lua, 6);
		}

		draw->rectangle(_pos1, _pos2, _color, _outline);
	}
	else error("[ERROR::LUA::RECT] Invalid arguments!");

	return 0;
}

//TRI(x1, y1, x2, y2, x3, y3, color, [outline])
int scripting::TRI(lua_State *_lua) {
	int argc = lua_gettop(_lua);

	if (argc >= 7) {
		if (argc > 8) {
			error("[ERROR::LUA::RECT] Invalid arguments!");
		}
		vec _pos1, _pos2, _pos3;
		_pos1.x = (float)lua_tonumber(_lua, 1);
		_pos1.y = (float)lua_tonumber(_lua, 2);
		_pos2.x = (float)lua_tonumber(_lua, 3);
		_pos2.y = (float)lua_tonumber(_lua, 4);
		_pos3.x = (float)lua_tonumber(_lua, 5);
		_pos3.y = (float)lua_tonumber(_lua, 6);
		byte  _color = (byte)lua_tonumber(_lua, 7);

		bool _outline = false;
		if (argc >= 8) {
			_outline = lua_toboolean(_lua, 8);
		}

		draw->triangle(_pos1, _pos2, _pos3, _color, _outline);
	}
	else error("[ERROR::LUA::RECT] Invalid arguments!");

	return 0;
}
