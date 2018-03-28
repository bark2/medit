#ifndef LINE_H
#define LINE_H

#include <cstdint>
#include <string>
#include <cstring>
#include <memory>
#include <utility>
#include <list>
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
    ~Line() = default;
    void push_char(char c);
    void emplace_char(char c, uint32_t offset);
    bool pop_char();
    bool erase_char(uint32_t offset);
    void push_string(const char* s, size_t size);    
    void push_string(std::string s);
    void emplace_string(uint32_t offset, const char* s, size_t size);
    bool start_offset(uint32_t& offset) const;
    bool next_from_offset(uint32_t offset, uint32_t& next) const;
    bool prev_from_offset(uint32_t offset, uint32_t& prev);
    bool move_cursor_by_offset(int32_t offset);
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
};

#endif
