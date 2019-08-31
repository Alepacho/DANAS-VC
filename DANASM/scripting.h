#pragma once
#include "defs.h"
#include <lua.hpp>
#include "drawing.h"

class scripting {
private:
	lua_State *_lua;
	std::string code;
	//static std::string str_error;
	
public:
	scripting();
	~scripting();
	
	void load_builtins();
	void load_functions();

	//
	void init(drawing &_draw, buffers &_memory);
	void create(std::vector<std::string> _code);
	void destroy();
	bool execute();

	static void error(std::string _text);
	
	std::string get_error();

	//MEMORY
	static int PEEK(lua_State *_lua);
	static int POKE(lua_State *_lua);

	//GRAPHICS
	static int TEXT(lua_State *_lua);	//DRAW TEXT
	static int CLS (lua_State *_lua);	//CLEAR SCREEN
	static int PSET(lua_State *_lua);	//DRAW PIXEL
	static int LINE(lua_State *_lua);	//DRAW LINE
	static int CIRC(lua_State *_lua);	//DRAW CIRCLE
	static int RECT(lua_State *_lua);	//DRAW RECTANGLE
	static int TRI (lua_State *_lua);	//DRAW TRIANGLE
	static int SPR (lua_State *_lua);	//DRAW SPRITE
	
};

