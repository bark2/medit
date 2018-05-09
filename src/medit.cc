#include <iostream>
#include <cctype>
#include <cassert>

#include "render.hh"
#include "types.hh"
#include "buffer.hh"

#define KEY_ESC 27

Buffer buffer;
bool end_window = false;

enum Mode {
    INSERT_MODE = 0,
    COMMAND_MODE,
    MARK_MODE
};
Mode mode = INSERT_MODE;


void key_callback(uint32_t key) {
    if (1) {
	if (mode == INSERT_MODE) {
	    switch (key) {
	    case '\n':
		buffer.emplace_line();
		break;
	    default:
		if (key < 127) {
		    if (key == KEY_ESC) {
			int32_t c = getch();
			if (c == ' ') {
			    mode = COMMAND_MODE;
			    break;
			}
		    }
		    buffer.emplace_char(key);
		    break;
		}
	    }
	} else if (mode == COMMAND_MODE) {
	    switch(key) {
	    case '\n':
		buffer.emplace_line();
		break;
	    case 'd':
		buffer.cursor_back_erase();
		break;
	    case '5':
		buffer.cursor_forward_erase();
		break;
	    case 'e':
		buffer.back_erase_word(buffer.ccursor());
		break;
	    case 'r':
		buffer.forward_erase_word(buffer.ccursor());
		break;
	    case 'l':
		buffer.cursor_next();
		break;
	    case 'j':
		buffer.cursor_prev();
		break;
	    case 'i':
		buffer.cursor_up();
		break;
	    case 'k':
		buffer.cursor_down();
		break;
	    case 'o':
		buffer.cursor_next_end_word();
		break;
	    case 'u':
		buffer.cursor_prev_start_word();
		break;	
	    case 'f':
		mode = INSERT_MODE;
		break; 
	    case 'q':
		end_window = true;
		break;
	    
	    }
	}
    }
}

int main()
{
    if (!Render::init())
	assert(0);
    uint32_t c;
    while (!end_window) {
	c = getch();
	if (c != ERR)
	    key_callback(c);

	Render::clear();
	Render::redraw_buffer(buffer);
	Render::flip();
    }
    
    Render::end();
    return 0;
}

