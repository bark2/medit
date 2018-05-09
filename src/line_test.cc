#include "line.hh"
#include <iostream>

int main()
{
    Line l = Line();
    l.emplace_string(l.cursor, "1234", 4);
    std::cout << l << "\n";
    l.cursor_prev();
    l.cursor_prev();
    l.emplace_char('5', l.cursor);
    std::cout << l << "\n";
    l.backspace(l.cursor);
    std::cout << l << "\n";
    l.backspace(l.cursor);
    std::cout << l << "\n";
    l.move_gap_to_dest(0);
    std::cout << l << "\n";
    l.emplace_char('6', 0);
    l.emplace_string(l.nchars, "789", 3);
    std::cout << l << "\n";
    return 0;
}
