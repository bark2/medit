#include "buffer.hh"

void Buffer::emplace_char(char c)
{
    cline->emplace_char(c, cline->cursor);
}

void Buffer::erase_char()
{
    if (!cline->erase_char(cline->cursor)) {
	if (cline == lines.cbegin())
	    return;
	std::list<Line>::iterator tmp = cline--;
	uint32_t pnchars = cline->nchars;
	cline->yank(cline->cursor, copy_line(*tmp));
	lines.erase(tmp);
	cline->cursor = pnchars;
    }
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

void Buffer::cursor_next()
{
    auto ti = cline;
    if (!cline->cursor_next() && ++ti != lines.end()) {
	cline++;
	cline->cursor = 0;
    }
}

void Buffer::cursor_prev()
{
    if (!cline->cursor_prev() && cline != lines.begin()) {
	cline--;
	cline->cursor = cline->nchars;
    }
}

void Buffer::cursor_up()
{
    uint32_t pcursor = cline->cursor;
    if (cline == lines.begin())
	return;
    cline--;
    if (cline->nchars < pcursor) {
	cline->cursor = cline->nchars;
    } else {
	cline->cursor = pcursor;
    }
}

void Buffer::cursor_down()
{
    uint32_t pcursor = cline->cursor;
    if (++cline == lines.end()) {
	cline--;
	return;
    }
    if (cline->nchars < pcursor) {
	cline->cursor = cline->nchars;
    } else {
	cline->cursor = pcursor;
    }
}
