#ifndef TTRE_LEXER_H
#include "tokens.h"
#include <iterator> 
#include <string_view>
#include <cstddef>  

namespace TTre
{
    struct LexerIterator;

    class Lexer
    {
    public:
        Lexer(std::string_view str) : source_(str),
            first_(Token::T_UNKNOWN),
            current_(Token::T_UNKNOWN),
            last_(Token::T_UNKNOWN)
        {}

        using iterator = LexerIterator;

        inline Token operator*() { return this->current_; }

        LexerIterator begin();
        LexerIterator end();

    private:
        unsigned int location_ = 0;
        Token get_next_token_();
        std::string_view source_;
        Token first_;
        Token current_;
        Token last_;
    };

    struct LexerIterator
    {
        using LexerIterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Token;
        using pointer = Token*;  // or also value_type*
        using reference = Token&;  // or also value_type&

        reference inline operator*() const { return *m_ptr; }
        pointer inline operator->() const { return m_ptr; }

        LexerIterator& operator++();

        LexerIterator operator++(int);

        friend inline bool operator== (const LexerIterator& a, const LexerIterator& b) { return a.m_ptr == b.m_ptr; };
        friend inline bool operator!= (const LexerIterator& a, const LexerIterator& b) { return a.m_ptr != b.m_ptr; };

        LexerIterator(pointer ptr) : m_ptr(ptr) {}
    private:
        pointer m_ptr;
    };

} // namespace TTre

#endif // TTRE_LEXER_H