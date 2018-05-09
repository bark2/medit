#include "buffer.hh"
#include <iostream>
#include <fstream>

inline uint32_t min(uint32_t x, uint32_t y) { return (x < y)? x : y;}

void Buffer::emplace_char(char c)
{
    cline->emplace_char(c, cline->cursor);
}

void Buffer::cursor_back_erase()
{
    uint32_t csr = ccursor();
    back_erase(cline, csr);
    cline->cursor = csr;
}

bool Buffer::back_erase(LineIterator& li, uint32_t& offset)
{
    if (!offset) {
	if (li == lines.cbegin())
	    return false;
	std::list<Line>::iterator prev_line = li--;
	uint32_t prev_nchars = (li)->nchars;
	li->yank(li->nchars, copy_line(*prev_line));
	lines.erase(prev_line);
	li->cursor = prev_nchars;
    } else
	li->backspace(offset);
    return true;
}

void Buffer::cursor_forward_erase()
{
    uint32_t csr = ccursor();
    forward_erase(cline, csr);
    cline->cursor = csr;
}

// erases char in offset's place
bool Buffer::forward_erase(LineIterator& li, uint32_t& offset)
{
    next(li, offset);
    next(li, offset);
    bool res = back_erase(li, offset);
    prev(li, offset);
    return res;
}

void Buffer::emplace_line() {
    Line& pline = *cline;
    cline++;
    lines.insert(cline, std::move(Line()));
    cline--;
    cline->yank(0, cut(pline, pline.cursor, pline.nchars - pline.cursor));
    cline->cursor = 0;
}

uint32_t Buffer::ccursor() const
{
    return cline->cursor;
}

bool Buffer::cursor_next()
{
    uint32_t csr = ccursor();
    bool res = next(cline, csr);
    cline->cursor = csr;
    return res;
}

bool Buffer::next(LineIterator& li, uint32_t& offset)
{
    if (offset == li->nchars) {
	if (std::next(li, 1) == lines.end())
	    return false;
	li++;
	offset = 0;
    } else
	li->next(offset);
    
    return true;
}

bool Buffer::cursor_prev()
{
    uint32_t csr = ccursor();
    bool res = prev(cline, csr);
    cline->cursor = csr;
    return res;
}

bool Buffer::prev(LineIterator& li, uint32_t& offset)
{
    if (!offset) {
	if (li == lines.begin())
	    return false;
	li--;
	offset = li->nchars;
    } else
	li->prev(offset);
    return true;
}

void Buffer::move_next_word(LineIterator& li, uint32_t& offset)
{
    while (!li->move_next_word(offset)) {
	li++;
	if (li == lines.end()) {
	    li--;
	    break;
	}
	offset = 0;
    }
}

void Buffer::next_end_word(LineIterator& li, uint32_t& offset)
{
    while (!li->next_end_word(offset)) {
	    li++;
	    if (li == lines.end()) {
		li--;
		break;
	    }
	    offset = 0;
	}
}

void Buffer::next_start_word(LineIterator& li, uint32_t& offset)
{
    while (!li->next_start_word(offset)) {
	    li++;
	    if (li == lines.end()) {
		li--;
		break;
	    }
	    offset = 0;
	}
}

void Buffer::cursor_next_end_word()
{
    uint32_t cursor = ccursor();
    next_end_word(cline, cursor);
    cline->cursor = cursor;
}

void Buffer::move_prev_word(LineIterator& li, uint32_t& offset)
{
    while (!li->move_prev_word(offset) && li != lines.begin()) {
	--li;
	offset = li->nchars;
    }
}

void Buffer::prev_end_word(LineIterator& li, uint32_t& offset)
{
    while (!li->prev_end_word(offset) && li != lines.begin()) {
	--li;
	offset = li->nchars;
    }
}

void Buffer::prev_start_word(LineIterator& li, uint32_t& offset)
{
    while (!li->prev_start_word(offset) && li != lines.begin()) {
	--li;
	offset = li->nchars;
    }
}

void Buffer::cursor_prev_start_word()
{
    uint32_t cursor = cline->cursor;
    prev_start_word(cline, cursor);
    cline->cursor = cursor;
}

void Buffer::cursor_up()
{
    uint32_t prev_cursor = cline->cursor;
    if (cline == lines.begin())
	return;
    cline--;
    cline->cursor = min(cline->nchars, prev_cursor);
}

void Buffer::cursor_down()
{
    uint32_t prev_cursor = cline->cursor;
    if (++cline == lines.end()) {
	cline--;
	return;
    }
    cline->cursor = min(cline->nchars, prev_cursor);
}

void Buffer::back_erase_word(uint32_t offset)
{
    LineIterator li = cline;
    prev_end_word(li, offset);
    while (cline != li || cline->cursor != offset) {
	cursor_back_erase();
    }
}

void Buffer::forward_erase_word(uint32_t offset)
{
    LineIterator li = cline;
    cursor_next_end_word();
    while (cline != li || cline->cursor != offset) {
	cursor_back_erase();
    }
}
