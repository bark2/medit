#ifndef LINE_H
#define LINE_H

#include <cstdint>
#include <string>
#include <cstring>
#include <memory>
#include <utility>
#include <list>
#include <ostream>
#include <queue>
#include "types.hh"

enum Style {
    ST_NORMAL = 0,
    ST_MARKED
};

struct Line {
    static const uint32_t defaultBufSize = 16;
    static const uint32_t defaultGapSize = 16;

    size_t size;
    uint32_t gap_offset;
    size_t gap_size;
    uint32_t nchars;

    std::unique_ptr<char[]> line;
    std::unique_ptr<Style[]> styles;
    
    uint32_t cursor; // todo: change this into the char number cursor is pointing to .
    
    void move_gap_to_dest(uint32_t offset);
    void expand();
    void shrink();

    Line(uint32_t bufSize=defaultBufSize, uint32_t gapSize=defaultGapSize);
    Line(Line&& other);
    friend std::ostream& operator<<(std::ostream& os, Line& l);
    ~Line() = default;
    void push_char(char c);
    void emplace_char(char c, uint32_t offset);
    bool pop_char();
    
    bool backspace(uint32_t offset);
    uint32_t erase_word(uint32_t offset);

    void push_string(const char* s, size_t size);    
    void push_string(std::string s);
    void emplace_string(uint32_t offset, const char* s, size_t size);
    uint32_t start_offset() const;
    bool next_from_offset(uint32_t offset, uint32_t& next) const;
    bool prev_from_offset(uint32_t offset, uint32_t& prev);
    bool move_by_offset(uint32_t pos, int32_t offset, uint32_t& new_offset);
    uint32_t operator[](size_t offset) const;
    bool get_at_pos(uint32_t pos, uint32_t& res) const;
    // void move_cursor_to_dest(uint32_t dest);
    bool cursor_next();
    bool cursor_prev();
    

    friend uint32_t mark(const Line& l, uint32_t offset, size_t n);
    friend std::string copy(const Line& l, uint32_t offset, size_t n);
    friend std::string copy_line(Line& l);
    friend std::string cut(Line& l, uint32_t offset, size_t n);
    void yank(uint32_t offset, std::list<std::string> sl);
    void yank(uint32_t offset, std::string s);
    bool move_next_word(uint32_t& offset);
    bool move_prev_word(uint32_t& offset);
    bool cursor_next_word();
    bool cursor_prev_word();

    bool next(uint32_t& offset);
    bool prev(uint32_t& offset);
    bool next_end_word(uint32_t& offset);
    bool next_start_word(uint32_t& offset);
    bool prev_end_word(uint32_t& offset);
    bool prev_start_word(uint32_t& offset);
};

#endif
