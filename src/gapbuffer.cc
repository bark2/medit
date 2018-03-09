#include "../include/gap_buffer.hh" // 
#include <cassert>

void Buffer::insert(char c)
{
    cline->putc(c);
}

void Buffer::backspace()
{
    if (!cline->cursor_offset && cline != lines.begin())
	cline--;
    else
	cline->rem();
}

void Buffer::moveCursor(int offset) {
    cline->moveCursor(offset);
}

void Buffer::insertLine() {
    Line& pline = *cline;
    cline->moveGapToDest(cline->cursor_offset);
    
    cline++;
    lines.insert(cline, std::move(Line()));
    cline--;
    if (pline.cursor_offset + pline.gap_size < pline.size &&
	pline.line[pline.cursor_offset + pline.gap_size] != '\0') {
	int start_offset = pline.cursor_offset + pline.gap_size;
	cline->puts(&pline.line[start_offset], start_offset - pline.size);
    }
}

void Buffer::combineLines() {
    assert(cline->cursor_offset == 0);
    int first_text_size = cline->gap_offset;
    int second_text_offset = cline->gap_offset + cline->gap_size;
    int second_text_size = cline->gap_offset + cline->gap_size - cline->size;
    cline--;
    cline->moveCursorToDest(cline->nchars + cline->gap_size);
    cline->puts(cline->line.get(), first_text_size);
    cline->puts(&cline->line[second_text_offset], second_text_size);
}

void Line::begin_iter() {
    i = 0;
    lchars = nchars;
}

bool Line::next_iter(char** c, Style** style)
{
    if (!nchars)
	return false;
    if (i == gap_offset)
	i += gap_size;
    if (!lchars || i == size) {
	lchars = nchars;
	i = 0;
	return false;
    }
    lchars--;
    *c = &line[i];
    *style = &styles[i];
    i++;
    return true;
}

bool Line::prev_char()
{
    
}

Line::Line(uint32_t lineSize, uint32_t gapSize)
    : line(std::make_unique<char[]>(lineSize)), styles(std::make_unique<Style[]>(lineSize)), size(lineSize), cursor_offset(0), gap_size(gapSize), gap_offset(0), nchars(0), i(0)
{}

Line::Line(Line&& other) :
    size(other.size), gap_offset(other.gap_offset), gap_size(other.gap_size), cursor_offset(other.cursor_offset), nchars(0), i(0) {
    line = std::move(other.line);
    styles = std::move(other.styles);
}

void Line::moveGapToDest(uint32_t offset) {
    int32_t delta = offset - gap_offset;
    if (delta < 0) {
	std::memcpy(line.get() + offset + gap_size, line.get() + offset, -1 * delta);
	gap_offset += delta;
    } else if (delta > 0) {
	std::memcpy(line.get() + gap_offset, line.get() + gap_offset + gap_size, delta);
	gap_offset += delta;
    }
}

void Line::expand() {
    uint32_t newLineSize = 2 * size;
    std::unique_ptr<char[]> newLine(new char[newLineSize]);
    std::memcpy(newLine.get(), line.get(), size);
    line = std::move(newLine);
    gap_size = size;
    gap_offset = size;
    size = newLineSize;
}

void Line::shrink()
{
    uint32_t newLineSize = size/2;
    std::unique_ptr<char[]> newLine(new char[newLineSize]);
    std::memcpy(newLine.get(), line.get(), size);
    line = std::move(newLine);
    size = newLineSize;
}

bool Line::moveCursor(int32_t offset) {
}

void Line::moveCursorToDest(uint32_t offset) {
    cursor_offset = offset;
}

void Line::putc(char c) {
    if (!gap_size)
	expand();
    moveGapToDest(cursor_offset);
    cursor_offset++;
    line[gap_offset++] = c;
    gap_size--;
    nchars++;
}

// todo: add a shrink() posibilty
void Line::rem() {
    // if (gap_size == (size+1)/2)
    // 	shrink();
    moveGapToDest(cursor_offset);
    line[cursor_offset] = '\0';
    if (!cursor_offset)
	return;
    cursor_offset--;
    gap_offset--;
    gap_size++;
    nchars--;
}

void Line::puts(const char* s, size_t size)
{
    for (int i = 0; i < size; i++)
	putc(s[i]);
}

void Line::puts(std::string s)
{
    for (char c : s)
	putc(c);
}
