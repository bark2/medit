#include "line.hh"
#include <cassert>

Line::Line(uint32_t lineSize, uint32_t gapSize)
    : line(std::make_unique<char[]>(lineSize)), styles(std::make_unique<Style[]>(lineSize)), size(lineSize), cursor(0), gap_size(gapSize), gap_offset(0), nchars(0)
{}

Line::Line(Line&& other) :
    size(other.size), gap_offset(other.gap_offset), gap_size(other.gap_size), cursor(other.cursor), nchars(other.nchars) {
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
    uint32_t new_line_size = 2 * size;
    std::unique_ptr<char[]> new_line = std::make_unique<char[]>(new_line_size);
    std::memcpy(new_line.get(), line.get(), size);
    line = std::move(new_line);
    gap_size = size;
    gap_offset = size;
    size = new_line_size;
}

void Line::shrink()
{
    uint32_t newLineSize = size/2;
    std::unique_ptr<char[]> newLine(new char[newLineSize]);
    std::memcpy(newLine.get(), line.get(), size);
    line = std::move(newLine);
    size = newLineSize;
}

bool Line::next_from_offset(uint32_t offset, uint32_t& next) const
{
    if (offset == size || offset == gap_offset && offset + gap_size == size)
	return false;
    if (offset == gap_offset)
	next = offset + gap_size;
    else
	next = offset + 1;
    return true;
}

bool Line::prev_from_offset(uint32_t offset, uint32_t& prev)
{ 
    if (offset == 0)
	return false;
    if (offset == gap_offset + gap_size)
	offset -= gap_size;
    else
	prev = offset - 1;
    return true;
}

uint32_t Line::start_offset() const
{
    return (gap_offset)? 0 : gap_offset;
}

bool Line::cursor_next()
{
    return next_from_offset(cursor, cursor);
}

bool Line::cursor_prev()
{
    return prev_from_offset(cursor, cursor);
}

bool Line::move_cursor_by_offset(int32_t offset)
{
    bool delta = (offset > 0)? true : false;
    offset = (offset > 0)? offset : -1 * offset;
    bool res;
    for (int i = 0; i < offset; i++)
	res = (delta)?
	    cursor_next():
	    cursor_prev();
    return res;
}

bool Line::get_at_pos(uint32_t pos, uint32_t& res) const
{
    res = line[(pos < gap_offset)? pos : pos + gap_size];
    return (pos < nchars)? true : false;
}

void Line::emplace_char(char c, uint32_t offset) {
    if (!gap_size)
	expand();
    move_gap_to_dest(offset);
    line[gap_offset++] = c;
    cursor = gap_offset;
    gap_size--;
    nchars++;
}

bool Line::erase_char(uint32_t offset) {
    // if (gap_size == (size+1)/2)
    // 	shrink();
    if (!offset)
	return false;
    move_gap_to_dest(offset);
    gap_offset--;
    cursor = gap_offset;
    gap_size++;
    line[gap_offset] = '\0';
    nchars--;
    return true;
}

bool Line::pop_char()
{
    return erase_char(nchars);
}

void Line::emplace_string(uint32_t offset, const char* s, size_t size)
{
    for (int i = 0; i < size; i++)
	emplace_char(s[i], offset+i);
}

// return: number of chars left
uint32_t mark(const Line& l, uint32_t offset, size_t n)
{
    return (offset + n < l.nchars)? 0 : offset + n - l.nchars;
}

uint32_t min(uint32_t a, uint32_t b)
{
    return (a < b)? a : b;
}

// todo: probobaly a bug where this copies part of the gap
std::string copy(const Line& l, uint32_t offset, size_t n)
{
    if (offset + n < l.gap_offset || offset > l.gap_offset + l.gap_size)
	return std::string(&l.line[offset], n);
    else if (offset < l.gap_offset)
	return std::string(&l.line[offset], l.gap_offset - offset) +
	    std::string(&l.line[l.gap_offset + l.gap_size], min(n - l.gap_offset + offset, l.size - l.gap_offset + offset));
    else if (offset >= l.gap_offset)
	return std::string(&l.line[offset + l.gap_size], min(n, l.size - l.gap_offset - l.gap_size));
}

std::string copy_line(Line& l)
{
    return copy(l, 0, l.nchars);
}

std::string cut(Line& l, uint32_t offset, size_t n)
{
    l.move_gap_to_dest(offset+n);
    std::string res(&l.line[offset], n);
    while(n) {
	l.erase_char(offset+n--);
    }
    return res;
}

void Line::yank(uint32_t offset ,std::string s)
{
    emplace_string(offset, s.c_str(), s.length());
}

void Line::yank(uint32_t offset ,std::list<std::string> sl)
{
    for (const std::string s : sl) {
	emplace_string(offset, s.c_str(), s.length());
	offset += s.length();
    }
}
