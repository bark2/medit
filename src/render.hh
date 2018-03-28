#ifndef RENDER_H
#define RENDER_H

#include "ncurses.h"
#include "buffer.hh"

namespace Render
{
    bool init();
    void clear();
    bool load();
    void redraw_buffer(const Buffer& buffer);
    void flip();
    void end();
}

#endif
