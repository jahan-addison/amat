#pragma once

#include <stdexcept>
#include <string>

#include <amat/lexer.h>
#include <amat/parser.h>
#include <amat/tokens.h>

namespace amat {

/* Order of precedence: Highest to lowest - Associativity
  1) Parenthesis    - non-associative
  2) Kleene star    - left
  3) Concatenation  - left
  4) Union          - left
*/
enum class Operator : unsigned
{
    T_OPEN_PAREN = 4,
    T_CLOSE_PAREN = 4,
    T_KLEENE_STAR = 3,
    T_CONCAT = 2,
    T_UNION = 1,
    T_UNKNOWN = 0
};

constexpr Operator
get_order_from_operator_char(unsigned char c)
{
    switch (c) {
        case '(':
            return Operator::T_OPEN_PAREN;
        case ')':
            return Operator::T_CLOSE_PAREN;
        case '*':
            return Operator::T_KLEENE_STAR;
        case '.':
            return Operator::T_CONCAT;
        case '|':
            return Operator::T_UNION;
    }
    return Operator::T_UNKNOWN;
}

class Parser
{
  public:
    // using enum Operators;
    Parser(Parser const&) = default;
    explicit Parser(std::string_view str)
      : source_(str)
      , lexer_(str)
    {
    }

  public:
    std::string_view parse()
    {
        while (*this->lexer_++ != amat::Token::T_END) {
            auto token = *this->lexer_;
            switch (token) {
                case Token::T_CHAR:
                    this->parse_char_();
                    break;
                case Token::T_KLEENE_STAR:
                    this->parse_kleene_star_();
                    break;
                case Token::T_UNION:
                    this->parse_union_();
                    break;
                case Token::T_OPEN_PAREN:
                    this->parse_left_paren_();
                    break;
                case Token::T_CLOSE_PAREN:
                    this->parse_right_paren_();
                    break;

                default:
                    throw std::runtime_error("parse error: invalid operator");
            }
        }
        if (this->operators_.front() == '(' or this->operators_.back() == '(') {
            throw std::runtime_error("parse error: unclosed parenthesis pair");
        }

        while (this->operators_.length()) {
            unsigned char last =
              static_cast<unsigned char>(this->operators_.back());
            this->operators_.pop_back();
            this->output_.push_back(last);
        }
        return this->output_;
    }

  private:
    void parse_kleene_star_()
    {
        this->is_concat_ = false;
        this->operators_.push_back('*');
    }

    void parse_union_()
    {
        this->is_concat_ = false;
        if (this->operators_.length()) {
            if (get_order_from_operator_char(this->operators_.back()) >=
                  get_order_from_operator_char('|') and
                this->operators_.back() != '(') {
                unsigned char last =
                  static_cast<unsigned char>(this->operators_.back());
                this->operators_.pop_back();
                this->output_.push_back(last);
            }
        }
        this->operators_.push_back('|');
    }

    void parse_char_()
    {
        if (this->is_concat_) {
            if (this->operators_.length()) {
                if (get_order_from_operator_char(this->operators_.back()) >=
                      get_order_from_operator_char('.') and
                    this->operators_.back() != '(') {
                    unsigned char last =
                      static_cast<unsigned char>(this->operators_.back());
                    this->operators_.pop_back();
                    this->output_.push_back(last);
                }
            }
            this->operators_.push_back('.');
        } else if (this->lexer_.pointer() == this->source_.length() and
                   this->operators_.back() == '*') {
            this->operators_.push_back('.');
        }
        this->is_concat_ = true;
        this->output_.push_back(this->lexer_.scanner().value());
    }

    inline void parse_left_paren_() { this->operators_.push_back('('); }

    void parse_right_paren_()
    {
        while (this->operators_.length() and this->operators_.back() != '(') {
            auto last = this->operators_.back();
            this->operators_.pop_back();
            this->output_.push_back(last);
        }
        if (!this->operators_.length()) {
            throw std::runtime_error("parse error: unclosed parenthesis pair");
        }
        if (this->operators_.back() == '(') {
            this->operators_.pop_back();
        }
    }

  private:
    std::string_view source_;
    Lexer lexer_;
    bool is_concat_ = false;
    std::string output_{};
    std::string operators_{};
};

} // namespace amat
