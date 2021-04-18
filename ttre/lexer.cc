#include <ttre/lexer.h>
#include <cctype>

namespace TTre
{
    Token Lexer::get_next_token()
    {
        auto token = Token::T_UNKNOWN;
        auto read = this->source_[this->location_];
        if (this->current_)
        {
            this->last_ = this->current_;
        }
        if (this->location_ >= this->source_.length())
        {
            this->current_ = Token::T_END;
            return Token::T_END;
        }

        if (read == '*')
        {
            token = Token::T_KLEENE_STAR;
        }

        if (read == '|')
        {
            token = Token::T_UNION;
        }

        if (std::isalpha(static_cast<unsigned char>(read)))
        {
            token = Token::T_CHAR;
        }
        this->location_++;
        if (!this->first_)
        {
            this->first_ = token;
        }
        return token;
    }

} // namespace TTre