#include <ttre/include/lexer.h>

namespace TTre
{
    Lexer::Iterator Lexer::begin()
    {
        return Iterator(&first_);
    }
    Lexer::Iterator Lexer::end()
    {
        return Iterator(&last_);
    }

}