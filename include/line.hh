#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <cstdint>
#include <string>
#include <cstring>
#include <memory>
#include <utility>
#include <list>
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
    uint32_t cursor_offset;
    
    void move_gap_to_dest(uint32_t offset);
    void expand();
    void shrink();

    Line(uint32_t bufSize=defaultBufSize, uint32_t gapSize=defaultGapSize);
    Line(Line&& other);
    ~Line() = default;
    void put_char(char c);
    void emplace_char(char c);
    bool erase_char();
    bool pop_char();
    void put_string(const char* s, size_t size);    
    void put_string(std::string s);
    void emplace_string(const char* s, size_t size);
    void move_cursor_by_offset(int32_t offset);
    void move_cursor_to_dest(uint32_t dest);
    uint32_t start_offset();
    bool next_from_offset(uint32_t offset, uint32_t& next);
    bool prev_from_offset(uint32_t offset, uint32_t& prev);
    bool cursor_next();
    bool cursor_prev();
    friend void combine_lines(Line& dest, Line& src);
};

#endif
