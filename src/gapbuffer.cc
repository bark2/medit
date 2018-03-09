#include "../include/gap_buffer.hh" // 
#include <cassert>

void Buffer::insert(char c)
{
    cline->nchars++;
    cline->putc(c);
}

void Buffer::backspace()
{
    while (!cline->cursor_offset && cline != lines.begin())
	cline--;
    if (!cline->cursor_offset && cline == lines.begin())
	return;
    cline->nchars--;
    cline->rem();
}

void Buffer::moveCursor(int offset) {
    cline->moveCursor(offset);
}

void Buffer::insertLine() {
    GapBuffer& pline = *cline;
    cline->moveGapTo(cline->cursor_offset);
    
    cline++;
    lines.insert(cline, std::move(GapBuffer()));
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

const char* GapBuffer::nextChar() {
    if (i == gap_offset)
	i += gap_size;
    if (!lchars || i == size) {
	lchars = nchars;
	i = 0;
	return nullptr;
    }
    nchars--;
    i++;
    return &line[i-1];
}

GapBuffer::GapBuffer(uint32_t lineSize, uint32_t gapSize)
    : line(std::make_unique<char[]>(lineSize)), size(lineSize), cursor_offset(0), gap_size(gapSize), gap_offset(0), nchars(0), i(0)
{}

GapBuffer::GapBuffer(GapBuffer&& other) :
    size(other.size), gap_offset(other.gap_offset), gap_size(other.gap_size), cursor_offset(other.cursor_offset), nchars(0), i(0) {
    line = std::move(other.line);
}

void GapBuffer::moveGapTo(uint32_t offset) {
    int32_t delta = offset - gap_offset;
    if (delta < 0) {
	std::memcpy(line.get() + offset + gap_size, line.get() + offset, -1 * delta);
	gap_offset += delta;
    } else if (delta > 0) {
	std::memcpy(line.get() + gap_offset, line.get() + gap_offset + gap_size, delta);
	gap_offset += delta;
    }
}

void GapBuffer::expand() {
    uint32_t newLineSize = 2 * size;
    std::unique_ptr<char[]> newLine(new char[newLineSize]);
    std::memcpy(newLine.get(), line.get(), size);
    line = std::move(newLine);
    gap_size = size;
    gap_offset = size;
    size = newLineSize;
}

void GapBuffer::shrink()
{
    uint32_t newBufSize = size/2;
    std::unique_ptr<char[]> newLine(new char[newBufSize]);
    std::memcpy(newLine.get(), line.get(), size);
    line = std::move(newLine);
    size = newBufSize;
}

void GapBuffer::moveCursor(int32_t offset) {
    cursor_offset += offset;
}

void GapBuffer::moveCursorToDest(uint32_t offset) {
    cursor_offset = offset;
}

void GapBuffer::putc(char c) {
    if (!gap_size)
	expand();
    moveGapTo(cursor_offset);
    cursor_offset++;
    line[gap_offset++] = c;
    gap_size--;
}

// todo: add a shrink() posibilty
void GapBuffer::rem() {
    // if (gap_size == (size+1)/2)
    // 	shrink();
    moveGapTo(cursor_offset);
    line[cursor_offset] = '\0';
    if (!cursor_offset)
	return;
    cursor_offset--;
    gap_offset--;
    gap_size++;
}

void GapBuffer::puts(const char* s, size_t size)
{
    for (int i = 0; i < size; i++)
	putc(s[i]);
}

void GapBuffer::puts(std::string s)
{
    for (char c : s)
	putc(c);
}
