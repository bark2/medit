#include "line.hh"
#include "buffer.hh"
#include "render.hh"

bool Render::init()
{
    WINDOW* w = initscr();
    if (cbreak() == ERR || raw() == ERR || noecho() == ERR || nodelay(w, FALSE) == ERR) {
	return false;
    }
    use_default_colors();
    return true;
}

bool Render::load()
{
}

void Render::clear()
{
    erase();
}

void Render::redraw_buffer(const Buffer& buffer)
{
    uint32_t c;
    uint32_t crow, ccol;
    
    uint32_t row = 0;
    move(0, 0);
    for (const_LineIterator it = buffer.lines.begin(); it != buffer.lines.end(); it++) {
	uint32_t i;
	for (i = 0; i < it->nchars; i++) {
	    addch((*it)[i]);
	    if (it == buffer.cline && i == buffer.cline->cursor) {
		crow = row;
		ccol = i;
	    }
	}
	if (it == buffer.cline && i == buffer.cline->cursor) {
	    crow = row;
	    ccol = i;
	}
	move(++row, 0);
    }
    move(crow, ccol);
}

void Render::flip()
{
    refresh();
}

void Render::end()
{
    endwin();
}
