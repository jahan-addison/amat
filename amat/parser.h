#pragma once

#include <amat/lexer.h>
#include <amat/tokens.h>
#include <string>

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
    std::string_view parse();

  private:
    void parse_kleene_star_();
    void parse_char_();
    void parse_left_paren_();
    void parse_right_paren_();
    void parse_union_();
    void parse_group_();

  private:
    std::string_view source_;
    Lexer lexer_;
    bool is_concat_ = false;
    std::string output_{};
    std::string operators_{};
};

} // namespace amat
