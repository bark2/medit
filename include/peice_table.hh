#ifdef PEICE_TABLE_H
#define PEICE_TABLE_H

#include "gap_buffer.h"

struct line {
    float fp;
    uint32_t lines;
    std::string s;
    uint32_t* lp;
};

#endif
