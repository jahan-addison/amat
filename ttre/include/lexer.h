#ifndef TTRE_LEXER_H
#include "tokens.h"
#include <iterator> 
#include <cstddef>  

namespace TTre
{

    class Lexer
    {
    public:
        struct Iterator
        {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = Token;
            using pointer = Token*;  // or also value_type*
            using reference = Token&;  // or also value_type&

            reference inline operator*() const { return *m_ptr; }
            pointer inline operator->() const { return m_ptr; }

            Iterator& operator++() { m_ptr++; return *this; }

            Iterator inline operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            friend inline bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
            friend inline bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

            Iterator(pointer ptr) : m_ptr(ptr) {}
        private:
            pointer m_ptr;
        };
        Iterator begin();
        Iterator end();

    private:
        Token first_;
        Token current_;
        Token last_;
    };

} // namespace TTre

#endif // TTRE_LEXER_H