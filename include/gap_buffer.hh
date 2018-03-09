#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <cstdint>
#include <string>
#include <cstring>
#include <memory>
#include <utility>
#include <list>

enum Style {
    ST_NORMAL = 0,
    ST_INVERT
};

struct GapBuffer {
    static const uint32_t defaultBufSize = 16;
    static const uint32_t defaultGapSize = 16;

    size_t size;
    uint32_t gap_offset;
    size_t gap_size;
    uint32_t nchars;

    std::unique_ptr<char[]> line;
    std::unique_ptr<Style[]> styles;
    uint32_t cursor_offset;
    
    void moveGapTo(uint32_t offset);
    void expand();
    void shrink();

    GapBuffer(uint32_t bufSize=defaultBufSize, uint32_t gapSize=defaultGapSize);
    GapBuffer(GapBuffer&& other);
    ~GapBuffer() = default;
    void putc(char c);
    void rem();
    void puts(const char* s, size_t size);    
    void puts(std::string s);    
    void moveCursor(int32_t offset);
    void moveCursorToDest(uint32_t dest);

    uint32_t lchars;
    uint32_t i;
    const char* nextChar();
};

struct Buffer
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

    // struct NextWritable {
    // 	std::list<GapBuffer>::const_iterator crow;
    // 	uint32_t cleft_chars;
    // 	uint32_t coffset;
    // 	NextWritable(const std::list<Buffer>::const_iterator& iter) : citer(iter), coffset(0), cleft_chars(iter->gap_buffer.nchars) {};
    // 	const char* operator()(const std::list<Piece>::const_iterator& iter) {
    // 	    if (coffset == iter->gap_buffer.gap_offset)
    // 		coffset = iter->gap_buffer.gap_offset + iter->gap_buffer.gap_size;
    // 	    if (!cleft_chars || coffset == iter->gap_buffer.size) {
    // 		coffset = 0;
    // 		citer++;
    // 		cleft_chars = citer->gap_buffer.nchars;
    // 		return NULL;
    // 	    }
    // 	    const char* c = &iter->gap_buffer.buf[coffset];
    // 	    coffset++;
    // 	    return c;
    // 	}
    // };
    std::list<GapBuffer> lines;
    std::list<GapBuffer>::iterator cline;

    Buffer() : lines() {
	lines.push_back(std::move(GapBuffer()));
	cline = lines.begin();
    };
    void insertLine();
    void combineLines();
    void insert(char c);
    void backspace();
    void moveCursor(int offset);
    // thinking of implementing only the basic functuality here.
    // a struct {cursor_pos, current_line, font_size, number_of_rows_on_screen, marked_initial_pos, mark_mode} should be saved on main
    // RenderText would get the background glyph color as well when rendering in mark mode
};

#endif
