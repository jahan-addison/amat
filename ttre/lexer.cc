#include <ttre/lexer.h>
#include <cctype>

namespace TTre
{
    LexerIterator Lexer::begin()
    {
        return LexerIterator(&first_);
    }
    LexerIterator& LexerIterator::operator++()
    {
        // TODO
        return *this;
    }

    LexerIterator Lexer::end()
    {
        return LexerIterator(&last_);
    }

    Token Lexer::get_next_token_()
    {
        auto token = Token::T_UNKNOWN;
        auto read = this->source_[this->location_];

        if (this->location_ >= this->source_.length())
        {
            this->current_ = this->last_ = Token::T_END;
            return Token::T_END;
        }

        if (read == '*')
        {
            token = Token::T_KLEENE_STAR;
        }

        if (read == '|')
        {
            token = Token::T_KLEENE_STAR;
        }

        if (std::isalpha(static_cast<unsigned char>(read)))
        {
            token = Token::T_CHAR;
        }
        this->location_++;
        return token;
    }

} // namespace TTre