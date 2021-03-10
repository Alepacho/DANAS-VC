#ifdef __linux__
#include "SDL2/SDL.h"
#else
#include <SDL.h>
#endif

#include "buffers.h"
#include "defs.h"
#include "drawing.h"
#include "scripting.h"
#include "console.h"
#include "editor.h"

//
std::string title = "DANAS/VC!";

mouse _mouse;
keyboard _keyboard;
scripting _script;

//Console stuff
//

//Window Resolution
int WINDOW_WIDTH = 640; //640
int WINDOW_HEIGHT = 480; //480

float RATIO = fmin((float)WINDOW_WIDTH / (float)SCREEN_WIDTH, (float)WINDOW_HEIGHT / (float)SCREEN_HEIGHT);

//BUFFERS
buffers memory;
buffers bufscreen;
buffers copy_data; //keyboard stuff
buffers sys_spritemap;	//program sprite map

/*
[MEMORY MAP]
+----------+------------------------+-----------------+
|  OFFSET  |	NAME				|	SIZE		  |
+----------+------------------------+-----------------+
| 0x000000 |						|  192x128		  |
| ........ |  VRAM (SCREEN BUFFER)  |  or			  |
| 0x005FFF |						|  ‭24576‬ bytes	  |
+----------+------------------------+-----------------+
| 0x006000 |					    |  16x3			  |
| ........ |		 PALETTE		|  or			  |
| 0x00602F |					    |  48 bytes		  |
+----------+------------------------+-----------------+
| 0x006030 |	  BORDER COLOR		|  1 byte		  |
+----------+------------------------+-----------------+
| 0x006031 |					    |  128x128	      |
| ........ |	   SPRITE MAP		|  or			  |
| 0x00A030 |					    |  16384 bytes	  |
+----------+------------------------+-----------------+
| 0x00A031 |					    |  128x128	      |
| ........ |	 BACKGROUND MAP		|  or			  |
| 0x00E030 |					    |  16384 bytes	  |
+----------+------------------------+-----------------+

*/

class::console _console;
editor _editor;

//
bool debug_mode = false;
byte program_mode = mode::terminal;

//SDL STUFF
bool is_working = true;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *screen;

uint system_tick = 0;
uint system_tick_last = 0;

int main(int argv, char *argc[]) {
	printf("%s\n", "DANASM");

	//init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("%s\n", "FAILED TO INIT SDL!!!");
		return -1;
	} else printf("%s\n", "INIT SDL");

	//init Window
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window) {
		printf("%s\n", "UNABLE TO CREATE WINDOW!!!");
		return -2;
	} else printf("%s\n", "CREATE WINDOW");
	

	//init Renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		printf("%s\n", "UNABLE TO CREATE RENDERER!!!");
	} else printf("%s\n", "CREATE RENDERER");

	//main texture
	
	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!screen) {
		printf("%s\n", "UNABLE TO CREATE MAIN TEXTURE!!!");
	} else printf("%s\n", "CREATE TEXTURE");
	//
	SDL_ShowCursor(SDL_DISABLE);

	//
	palette standard;
	standard.count = 16;
	standard._color = new color[standard.count];

	/*black,	//0
	dkred,	//1
	red,	//2
	orange,	//3
	yellow,	//4
	white,	//5
	ltpink,	//6
	pink,	//7
	purple,	//8
	violet,	//9
	dkcyan,	//10
	cyan,	//11
	lime,	//12
	green,	//13
	blue,	//14
	dkblue	//15*/

	standard._color[_c::black]	= 0x16171a;
	standard._color[_c::dkred]	= 0x7f0622;
	standard._color[_c::red]	= 0xd62411;
	standard._color[_c::orange] = 0xff8426;
	standard._color[_c::yellow] = 0xffd100;
	standard._color[_c::white]	= 0xfafdff;
	standard._color[_c::ltpink] = 0xff80a4;
	standard._color[_c::pink]	= 0xff2674;
	standard._color[_c::purple] = 0x94216a;
	standard._color[_c::violet] = 0x430067;
	standard._color[_c::dkcyan] = 0x234975;
	standard._color[_c::cyan]	= 0x68aed4;
	standard._color[_c::lime]	= 0xbfff3c;
	standard._color[_c::green]	= 0x10d275;
	standard._color[_c::blue]	= 0x007899;
	standard._color[_c::dkblue] = 0x002859;

	//SPECIAL BUFFER FOR RENDERING
	bufscreen.create(VRAM_SIZE * 4);

	//
	MEMORY_SIZE = VRAM_SIZE + standard.count * 3 + 1 + SMAP_SIZE;
	memory.create(MEMORY_SIZE, buffer::type::fixed);

	//PALETTE
	for (int i = 0; i < standard.count; i++) {
		memory.poke(VRAM_SIZE + i * 3,		0, standard._color[i] >> 0);	//r
		memory.poke(VRAM_SIZE + i * 3 + 1,	0, standard._color[i] >> 8);	//g
		memory.poke(VRAM_SIZE + i * 3 + 2,	0, standard._color[i] >> 16);	//b
	}

	//BACKGROUND COLOR
	memory.poke(VRAM_SIZE + standard.count * 3, 0, _c::black);

	drawing _draw(memory, VRAM_SIZE);
	//

	_script.init(_draw, memory);								//
	_console.init(_draw, console_input, editor_code, _script);	//
	_editor.init(_draw, editor_code);


	//
	while (is_working) {

		//
		SDL_RenderClear(renderer);

		//draw background
		col _bkg;
		byte _num = memory.peek(VRAM_SIZE + standard.count * 3);
		_bkg.b = memory.peek(VRAM_SIZE + _num * 3);
		_bkg.g = memory.peek(VRAM_SIZE + _num * 3 + 1);
		_bkg.r = memory.peek(VRAM_SIZE + _num * 3 + 2);

		//Border color
		SDL_SetRenderDrawColor(renderer, _bkg.r, _bkg.g, _bkg.b, 0xFF);

		system_tick += 1;// SDL_GetTicks();

		_mouse.pressed = false;
		_mouse.unpressed = false;
		_keyboard.copy = false;

		//update
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			//
			if (event.type == SDL_QUIT) {
				is_working = false;
			}
			//
			if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					printf("%s\n", "WINDOW RESIZED");
					SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);

					RATIO = fmin((float)WINDOW_WIDTH / (float)SCREEN_WIDTH, (float)WINDOW_HEIGHT / (float)SCREEN_HEIGHT);
					printf("WRES: %i, %i;\nSRES: %f, %f\n", WINDOW_WIDTH, WINDOW_HEIGHT, SCREEN_WIDTH * RATIO, SCREEN_HEIGHT * RATIO);
					printf("RATIO: %f\n", RATIO);
					//printf("(WW - SW)/4: %f\n", (WINDOW_WIDTH - SCREEN_WIDTH * RATIO) / 4);
				}
			}
			//
			if (event.type == SDL_MOUSEMOTION) {
				//get mouse position
				float _srw = (WINDOW_WIDTH / RATIO - SCREEN_WIDTH) / 2;
				float _srh = (WINDOW_HEIGHT / RATIO - SCREEN_HEIGHT) / 2;

				SDL_GetMouseState(&_mouse.x, &_mouse.y);
				_mouse.x = (int)((_mouse.x / RATIO) - _srw);
				_mouse.y = (int)((_mouse.y / RATIO) - _srh);
			}
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				_mouse.pressed = true;
				_mouse.pressing = true;
			}
			if (event.type == SDL_MOUSEBUTTONUP) {
				_mouse.pressing = false;
				_mouse.unpressed = true;
			}

			if (event.type == SDL_MOUSEWHEEL) {
				editor_code.offset.y = (float)clamp((int)editor_code.offset.y - event.wheel.y, 0, editor_code.text.size());
			}

			if (event.type == SDL_TEXTINPUT) {
				
				//
			}
			//
			if (event.type == SDL_KEYDOWN) {
				_keyboard.caps = event.key.keysym.mod & KMOD_CAPS;

				switch (program_mode) {
					case mode::edit: {
						if (_editor.section == section::coding) {
							uint _symbol = event.key.keysym.sym;
							if (_symbol >= 32 && _symbol <= 126) {
								//uint _char = (event.key.keysym.mod & KMOD_LSHIFT || keyboard::caps) ? symbol.find()
								std::string *_line = &editor_code.text[editor_code.line];
								if (event.key.keysym.mod & KMOD_LSHIFT || _keyboard.caps) {
									byte caps_symbol = shift[symbol.find(_symbol)];
									//_editor.insert(_char);
									_line->insert(_line->begin() + editor_code.col, caps_symbol);
								} else {
									//_editor.insert(_symbol);
									_line->insert(_line->begin() + editor_code.col, _symbol);
								}
								editor_code.col++;
							}
						}
					} break;
					case mode::terminal: {
						uint _symbol = event.key.keysym.sym;
						if (_symbol >= 32 && _symbol <= 126) {
							//uint _char = (event.key.keysym.mod & KMOD_LSHIFT || keyboard::caps) ? symbol.find()
							if (event.key.keysym.mod & KMOD_LSHIFT || _keyboard.caps) {
								byte _char = shift[symbol.find(_symbol)];
								console_input.insert(console_input.length(), 1, _char);
							} else console_input.insert(console_input.length(), 1, _symbol);
						}
					} break;
				}


				switch (event.key.keysym.sym) {
				case SDLK_BACKSPACE:
					switch (program_mode) {
						case mode::edit: {
							if (editor_code.text.size() > 0) {
								std::string *line = &editor_code.text[editor_code.line];
								uint _length = line->length();

								if (editor_code.col == 0) {			//если колонка в начале
									if (editor_code.line != 0) {	//если это не первая линия
										std::string *line_prev = &editor_code.text[editor_code.line - 1];
										if (line->length() > 0) {	//если строка не пустая
											//переносим всё в пред строку
											editor_code.col = line_prev->length();
											line_prev->insert(line_prev->length(), *line);
										} else editor_code.col = line_prev->length();
										//удаляем эту
										editor_code.text.erase(editor_code.text.begin() + editor_code.line);
										editor_code.line--;
										
									} else printf("Tried to delete first line...\n");
								} else {
									line->erase(line->begin() + editor_code.col - 1); //удаляем символ сзади
									editor_code.col--;
								}
							}
						} break;
						case mode::terminal: {
							if (console_input.length() > 0) console_input.erase(console_input.length() - 1);
						} break;
					}
					break;
				case SDLK_ESCAPE:
					program_mode = program_mode != mode::terminal ? program_mode = mode::terminal : program_mode = mode::edit;
					break;
				case SDLK_RETURN: //enter
					switch (program_mode) {
						case mode::terminal: {
							_console.command(console_input);
							
							console_input.clear();
						} break;
						case mode::edit: {
							if (_editor.section == section::coding) {
								std::string *line = &editor_code.text[editor_code.line];
								
								//создаем новую строку
								editor_code.text.insert(editor_code.text.begin() + editor_code.line + 1, "");
								//if (editor_code.col < line->length() && line->length() > 0) {	//если строка не пустая и колонка где-то не в конце, то добавляем текст после колонки
								if ((int)(line->length() - editor_code.col) > 0) {
									printf("piss len(%i) - col(%i) = %i\n", line->length(), editor_code.col, line->length() - editor_code.col);
									printf("sub: %s\n", line->substr(editor_code.col, line->length() - editor_code.col).c_str());

									std::string *line_next = &editor_code.text[editor_code.line + 1];
									std::string _sub = line->substr(editor_code.col, line->length() - editor_code.col);
									//printf("aye: %i\n", line->size() - editor_code.col);
									//printf("sub: '%s'\n", _sub.c_str());
									line_next->insert(0, _sub);
									line->erase(editor_code.col, line->length() - editor_code.col);
									
									
								}

								editor_code.col = 0;
								editor_code.line++;
							}
						} break;
					}
					break;
				case SDLK_TAB:
					switch (program_mode) {
						case mode::terminal: {
							console_input.insert(console_input.end(), 2, ' ');
						} break;
						case mode::edit: {
							if (_editor.section == section::coding) {
								std::string *line = &editor_code.text[editor_code.line];
								line->insert(editor_code.col, 2, ' ');
							}
						} break;
					}
				break;
				case SDLK_c:
					if (event.key.keysym.mod & KMOD_LCTRL) {
						_keyboard.copy = true;
					}
					break;
				case SDLK_v:
					if (event.key.keysym.mod & KMOD_LCTRL) {
						_keyboard.paste = true;
					}
					break;
				case SDLK_UP:
					switch (program_mode) {
						case mode::edit: {
							if (_editor.section == section::coding) {
								if (editor_code.line > 0) editor_code.line--;
								if (editor_code.col > editor_code.text[editor_code.line].length()) editor_code.col = editor_code.text[editor_code.line].length();
							} 
						} break;
					}
					break;
				case SDLK_DOWN:
					switch (program_mode) {
						case mode::edit: {
							if (_editor.section == section::coding) {
								if (editor_code.line < editor_code.text.size() - 1) editor_code.line++;
								if (editor_code.col > editor_code.text[editor_code.line].length()) editor_code.col = editor_code.text[editor_code.line].length();
							}
						} break;
					}
					break;
				case SDLK_LEFT:
					switch (program_mode) {
						case mode::edit: {
							if (_editor.section == section::coding) {
								//if (editor_code.col < editor_code.text.size() - 1) editor_code.col++; editor_code.text[editor_code.col]
								if (editor_code.col > 0) editor_code.col--; else {
									if (editor_code.line > 0) {
										editor_code.line--;
										editor_code.col = editor_code.text[editor_code.line].length();
									}
								}
							}
						} break;
					}
					break;
				case SDLK_RIGHT:
					switch (program_mode) {
						case mode::edit: {
							if (_editor.section == section::coding) {
								//if (editor_code.col < editor_code.text.size() - 1) editor_code.col++; editor_code.text[editor_code.col]
								if (editor_code.col < editor_code.text[editor_code.line].length()) editor_code.col++; else {
									if (editor_code.line < editor_code.text.size() - 1) {
										editor_code.line++;
										editor_code.col = 0;
									}
								}
								//printf("col: %i\nlen: %i\n", editor_code.col, editor_code.text[editor_code.line].length());
							}
						} break;
					}
					break;
				}
			}
		}

		switch (program_mode) {
			case mode::execute: {
				//program_mode = mode::terminal;
				//_console.insert("&7Running game is currently unsupported...&5&n");
				//running script

				bool _check = _script.execute();
				if (_check) {
					program_mode = mode::terminal;
					_console.insert("&7" + _script.get_error() + "&5&n");
				}

			} break;
			case mode::terminal: {
				//render to texture
				_draw.fill(0);
				
				_console.render();
				
			} break;
			case mode::edit: {
				_editor.render();
			} break;
		}

		_draw.sprite(nvec((float)_mouse.x, (float)_mouse.y), 12, _c::dkblue, true);

		//SDL_UpdateTexture(screen, NULL, memory.get(), SCREEN_WIDTH * 4);

		for (uint i = 0; i < VRAM_SIZE; i++) {
			byte _indx = memory.peek(i);
			col _col;
			_col.r = memory.peek(VRAM_SIZE + _indx * 3);
			_col.g = memory.peek(VRAM_SIZE + _indx * 3 + 1);
			_col.b = memory.peek(VRAM_SIZE + _indx * 3 + 2);

			bufscreen.poke(i * 4, 0,	 _col.r);
			bufscreen.poke(i * 4 + 1, 0, _col.g);
			bufscreen.poke(i * 4 + 2, 0, _col.b);
		}
		

		int _pitch = 0; void *pixels;
		SDL_LockTexture(screen, NULL, &pixels, &_pitch);
		memcpy(pixels, bufscreen.get(), VRAM_SIZE * 4);
		SDL_UnlockTexture(screen);

		//draw framebuffer
		SDL_Rect spos;
		spos.x = (int)(WINDOW_WIDTH - SCREEN_WIDTH * RATIO) / 2;
		spos.y = (int)(WINDOW_HEIGHT - SCREEN_HEIGHT * RATIO) / 2;
		spos.w = (int)(SCREEN_WIDTH * RATIO);
		spos.h = (int)(SCREEN_HEIGHT * RATIO);

		SDL_RenderCopy(renderer, screen, NULL, &spos);
		SDL_Rect _viewport = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
		SDL_RenderSetViewport(renderer, &_viewport);
		SDL_RenderPresent(renderer);
	}

	_console.clean();
	_editor.clean();
	copy_data.destroy();
	bufscreen.destroy();
	memory.destroy();
	//_script.destroy();

	SDL_DestroyTexture(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	system("pause");

	return 0;
}
