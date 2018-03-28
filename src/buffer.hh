#ifndef BUFFER_H
#define BUFFER_H

#include "line.hh"

struct Buffer
{

    std::list<Line> lines;
    std::list<Line>::iterator cline;

    struct marked_el {
	Line& l;
	uint32_t offset;
	size_t n;
    };
    typedef std::queue<marked_el> Marker;
    Marker marker;
    
    Buffer() : lines() {
    	lines.push_back(std::move(Line()));
    	cline = lines.begin();
    };
    void emplace_char(char c);
    void emplace_line();
    void erase_char();
    uint32_t ccursor() const;
    void cursor_next();
    void cursor_prev();
    void cursor_up();
    void cursor_down();
    void move_cursor(uint32_t offset);
};

#endif
