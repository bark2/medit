#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <cstdint>
#include <string>
#include <cstring>
#include <memory>
#include <utility>
#include <list>

struct GapBuffer {
    static const unsigned int defaultBufSize = 16;
    static const unsigned int defaultGapSize = 16;

    unsigned int size;
    unsigned int gap_offset;
    unsigned int gap_size;
    unsigned int nchars;
    
    void moveGapTo(unsigned int offset);
    void expand();
    void shrink();

    struct Char {char c; Style style;};
    std::unique_ptr<char[]> buf;
    unsigned int cursor_offset;

    GapBuffer(unsigned int bufSize=defaultBufSize, unsigned int gapSize=defaultGapSize);
    GapBuffer(GapBuffer&& other) :
	size(other.size), gap_offset(other.gap_offset), gap_size(other.gap_size), cursor_offset(other.cursor_offset), nchars(0) {
	buf = std::move(other.buf);
    }
    ~GapBuffer() = default;
    void putc(char c);
    void rem();
    void puts(const char* s, size_t size);    
    void puts(std::string s);    
    void moveCursor(int offset);
    void moveCursorTo(uint32_t offset);
};

struct Piece {
    GapBuffer gap_buffer;
    // bool writeable;
};

struct PieceTable
{
    // turning this into a clojure(well it is already is, mb lambda), to mark a region for a command, e.g cut,copy,render..
    // mark(start_line, end_line, start_char, end_char) --> struct {const char* s, size_t size}
    // marking shoudln't change the cursor position
    // after ill use a rb_tree to search for a specific line index the search will be much faster..
    // mb will use hash table with a linked list for O(1) search/insert/remove
    // should use a rope to search for special words, influential in syntax highlighting(mb not because this should be used on really small files ..)
    // thus we should use a new ds for characters: {char c, word_mode}, where word_mode specifies if the word is
    // a saved word, struct, class, type, variable_name ..
    // idie - collabration programming editor. how should we support that? P2P\clinet server? undo operation? real time?
    int start_mark;
    int end_mark;
    struct NextWritable {
	std::list<Piece>::const_iterator citer;
	unsigned int cleft_chars;
	unsigned int coffset;
	NextWritable(const std::list<Piece>::const_iterator& iter) : citer(iter), coffset(0), cleft_chars(iter->gap_buffer.nchars) {};
	const char* operator()(const std::list<Piece>::const_iterator& iter) {
	    if (coffset == iter->gap_buffer.gap_offset)
		coffset = iter->gap_buffer.gap_offset + iter->gap_buffer.gap_size;
	    if (!cleft_chars || coffset == iter->gap_buffer.size) {
		coffset = 0;
		citer++;
		cleft_chars = citer->gap_buffer.nchars;
		return NULL;
	    }
	    const char* c = &iter->gap_buffer.buf[coffset];
	    coffset++;
	    return c;
	}
    };
    
    std::list<Piece> pieces;
    std::list<Piece>::iterator citer;

    PieceTable() : pieces() {
	pieces.push_back(std::move(Piece()));
	citer = pieces.begin();
    };
    void insertPiece();
    void combinePieces();
    void insert(char c);
    void backspace();
    void moveCursor(int offset);
    // thinking of implementing only the basic functuality here.
    // a struct {cursor_pos, current_line, font_size, number_of_rows_on_screen, marked_initial_pos, mark_mode} should be saved on main
    // RenderText would get the background glyph color as well when rendering in mark mode
};

#endif
