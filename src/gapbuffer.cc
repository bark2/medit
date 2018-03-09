#include "../include/gap_buffer.hh" // 
#include <cassert>

void PieceTable::insert(char c)
{
    citer->gap_buffer.nchars++;
    citer->gap_buffer.putc(c);
}

void PieceTable::backspace()
{
    while (!citer->gap_buffer.cursor_offset && citer != pieces.begin())
	citer--;
    if (!citer->gap_buffer.cursor_offset && citer == pieces.begin())
	return;
    citer->gap_buffer.nchars--;
    citer->gap_buffer.rem();
}

void PieceTable::moveCursor(int offset) {
    citer->gap_buffer.moveCursor(offset);
}

void PieceTable::insertPiece() {
    GapBuffer& pre_gap_buffer = citer->gap_buffer;
    citer->gap_buffer.moveGapTo(citer->gap_buffer.cursor_offset);

    citer++;
    pieces.insert(citer, std::move(Piece()));
    citer--;
    if (pre_gap_buffer.cursor_offset + pre_gap_buffer.gap_size < pre_gap_buffer.size &&
	pre_gap_buffer.buf[pre_gap_buffer.cursor_offset + pre_gap_buffer.gap_size] != '\0') {
	int start_offset = pre_gap_buffer.cursor_offset + pre_gap_buffer.gap_size;
	citer->gap_buffer.puts(&pre_gap_buffer.buf[start_offset], start_offset - pre_gap_buffer.size);
    }
}

void PieceTable::combinePieces() {
    assert(citer->gap_buffer.cursor_offset == 0);
    int first_text_size = citer->gap_buffer.gap_offset;
    int second_text_offset = citer->gap_buffer.gap_offset + citer->gap_buffer.gap_size;
    int second_text_size = citer->gap_buffer.gap_offset + citer->gap_buffer.gap_size - citer->gap_buffer.size;
    citer--;
    citer->gap_buffer.moveCursorTo(citer->gap_buffer.nchars + citer->gap_buffer.gap_size);
    citer->gap_buffer.puts(citer->gap_buffer.buf.get(), first_text_size);
    citer->gap_buffer.puts(&citer->gap_buffer.buf[second_text_offset], second_text_size);
}

GapBuffer::GapBuffer(uint32_t bufSize, uint32_t gapSize)
    : buf(std::make_unique<char[]>(bufSize)), size(bufSize), cursor_offset(0), gap_size(gapSize), gap_offset(0)
{}

void GapBuffer::moveGapTo(uint32_t offset) {
    int32_t delta = offset - gap_offset;
    if (delta < 0) {
	std::memcpy(buf.get() + offset + gap_size, buf.get() + offset, -1 * delta);
	gap_offset += delta;
    } else if (delta > 0) {
	std::memcpy(buf.get() + gap_offset, buf.get() + gap_offset + gap_size, delta);
	gap_offset += delta;
    }
}

void GapBuffer::expand() {
    uint32_t newBufSize = 2 * size;
    std::unique_ptr<char[]> newBuf(new char[newBufSize]);
    std::memcpy(newBuf.get(), buf.get(), size);
    buf = std::move(newBuf);
    gap_size = size;
    gap_offset = size;
    size = newBufSize;
}

void GapBuffer::shrink()
{
    uint32_t newBufSize = size/2;
    std::unique_ptr<char[]> newBuf(new char[newBufSize]);
    std::memcpy(newBuf.get(), buf.get(), size);
    buf = std::move(newBuf);
    size = newBufSize;
}

void GapBuffer::moveCursor(int offset) {
    cursor_offset += offset;
}

void GapBuffer::moveCursorTo(uint32_t offset) {
    cursor_offset = offset;
}

void GapBuffer::putc(char c) {
    if (!gap_size)
	expand();
    moveGapTo(cursor_offset);
    cursor_offset++;
    buf[gap_offset++] = c;
    gap_size--;
}

// todo: add a shrink() posibilty
void GapBuffer::rem() {
    // if (gap_size == (size+1)/2)
    // 	shrink();
    moveGapTo(cursor_offset);
    buf[cursor_offset] = '\0';
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
