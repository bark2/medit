#ifndef BUFFER_H
#define BUFFER_H

#include "line.hh"
#include <assert.h>
typedef std::list<Line>::iterator  LineIterator;
typedef std::list<Line>::const_iterator  const_LineIterator;

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

    uint32_t ccursor() const;
    bool next(LineIterator& li, uint32_t& offset);
    bool prev(LineIterator& li, uint32_t& offset);
    bool cursor_next();
    bool cursor_prev();
    void next_end_word(LineIterator& li, uint32_t& offset);
    void next_start_word(LineIterator& li, uint32_t& offset);
    void prev_end_word(LineIterator& li, uint32_t& offset);
    void prev_start_word(LineIterator& li, uint32_t& offset);
    void cursor_prev_start_word();
    void cursor_next_end_word();
    void cursor_up();
    void cursor_down();
    
    void cursor_back_erase();
    void cursor_forward_erase();
    bool back_erase(LineIterator& li, uint32_t& offset);
    bool forward_erase(LineIterator& li, uint32_t& offset);
    void back_erase_word(uint32_t offset);
    void forward_erase_word(uint32_t offset);
    // bool back_erase_word(LineIterator& li, uint32_t& offset);
    // bool forward_erase_word(LineIterator& li, uint32_t& offset);

    void move_next_word(LineIterator& li, uint32_t& offset);
    void move_prev_word(LineIterator& li, uint32_t& offset);
};

#endif
