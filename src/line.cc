#include "../include/line.hh" // 
#include <cassert>

bool Line::cursor_next()
{
    return next_from_offset(cursor_offset, cursor_offset);
}

bool Line::cursor_prev()
{
    return prev_from_offset(cursor_offset, cursor_offset);
}

Line::Line(uint32_t lineSize, uint32_t gapSize)
    : line(std::make_unique<char[]>(lineSize)), styles(std::make_unique<Style[]>(lineSize)), size(lineSize), cursor_offset(0), gap_size(gapSize), gap_offset(0), nchars(0)
{}

Line::Line(Line&& other) :
    size(other.size), gap_offset(other.gap_offset), gap_size(other.gap_size), cursor_offset(other.cursor_offset), nchars(other.nchars) {
    line = std::move(other.line);
    styles = std::move(other.styles);
}

void Line::move_gap_to_dest(uint32_t dest) {
    int32_t delta = dest - gap_offset;
    if (delta < 0) {
	std::memcpy(line.get() + dest + gap_size, line.get() + dest, -1 * delta);
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

bool Line::next_from_offset(uint32_t offset, uint32_t& next)
{
    offset++;
    if (offset == size)
	return false;
    if (offset == gap_offset) {
	if (offset + gap_size == size)
	    return false;
	offset += gap_size;
    }
    next = offset;
    return true;
}

bool Line::prev_from_offset(uint32_t offset, uint32_t& prev)
{ 
    if (offset - 1 == 0)
	return false;
    if (offset == gap_offset + gap_size) {
	if (!gap_offset)
	    return false;
	offset -= gap_size;
    }
    prev = offset - 1;
    return true;
}

void Line::move_cursor_to_dest(uint32_t offset)
{
    cursor_offset = offset;
}

void Line::putc(char c) {
    if (!gap_size)
	expand();
    move_gap_to_dest(cursor_offset);
    cursor_offset++;
    line[gap_offset++] = c;
    gap_size--;
    nchars++;
}

// todo: add a shrink() posibilty
bool Line::rem() {
    // if (gap_size == (size+1)/2)
    // 	shrink();
    move_gap_to_dest(cursor_offset);
    line[cursor_offset] = '\0';
    if (!cursor_offset)
	return false;
    cursor_offset--;
    gap_offset--;
    gap_size++;
    nchars--;
    return true;
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

uint32_t Line::start_offset()
{
    return (!gap_offset)? gap_size : 0;
}

// todo: refactor into copy_from_line, and remove_from_line
void copy_line(Line& dest, Line& src) {
    assert(src.cursor_offset == 0);
    int first_text_size = src.gap_offset;
    int second_text_offset = src.gap_offset + src.gap_size;
    int second_text_size = src.gap_offset + src.gap_size - src.size;
    dest.move_cursor_to_dest(dest.nchars + dest.gap_size);
    dest.puts(dest.line.get(), first_text_size);
    dest.puts(&dest.line[second_text_offset], second_text_size);
}
