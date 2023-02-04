#pragma once

#include <cctype>
#include <optional>
#include <string_view>

#include <amat/tokens.h>

namespace amat {

class Lexer
{
  public:
    Lexer(Lexer const&) = default;
    explicit Lexer(std::string_view str)
      : source_(str)
      , pointer_(0)
      , current_(Token::T_UNKNOWN)
      , last_(Token::T_UNKNOWN)
    {
    }

  public:
    Token get_next_token()
    {
        Token token = Token::T_UNKNOWN;
        unsigned char read =
          static_cast<unsigned char>(this->source_[this->pointer_]);

        this->last_ = this->current_;

        if (this->pointer_ >= this->source_.length()) {
            this->current_ = Token::T_END;
            return Token::T_END;
        }

        switch (read) {
            case '*':
                token = Token::T_KLEENE_STAR;
                break;
            case '|':
                token = Token::T_UNION;
                break;
            case '(':
                token = Token::T_OPEN_PAREN;
                break;
            case ')':
                token = Token::T_CLOSE_PAREN;
                break;
        }

        if (std::isalpha(read))
            token = Token::T_CHAR;

        this->pointer_++;

        this->current_ = token;
        this->scanner_ = read;
        return token;
    }
    inline std::optional<unsigned char> scanner() const { return scanner_; }
    inline unsigned int pointer() { return pointer_; }

  public:
    Lexer operator++(int)
    {
        this->get_next_token();
        return *this;
    }
    inline Token operator*() const { return current_; }

  private:
    std::string_view source_;
    unsigned int pointer_;
    std::optional<unsigned char> scanner_;
    Token current_;
    Token last_;
};

} // namespace amat
