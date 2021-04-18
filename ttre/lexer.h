#ifndef TTRE_LEXER_H
#include "tokens.h"
#include <iostream> 
#include <string_view>
#include <cstddef>  

namespace TTre
{
    class Lexer
    {
    public:
        Lexer(std::string_view str) : source_(str),
            first_(Token::T_UNKNOWN),
            current_(Token::T_UNKNOWN),
            last_(Token::T_UNKNOWN)
        {}

        Token get_next_token();

    private:
        unsigned int location_ = 0;
        std::string_view source_;
        Token first_;
        Token current_;
        Token last_;
    };

} // namespace TTre

#endif // TTRE_LEXER_H