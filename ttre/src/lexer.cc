#include <ttre/include/lexer.h>

namespace TTre
{
    Lexer::Iterator Lexer::begin()
    {
        return Iterator(&first_);
    }
    Lexer::Iterator& Lexer::Iterator::operator++()
    {
        // TODO
        return *this;
    }

    Lexer::Iterator Lexer::end()
    {
        return Iterator(&last_);
    }

}