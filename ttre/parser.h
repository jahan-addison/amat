#ifndef TTRE_PARSER_H
#include <ttre/lexer.h>
#include <stack>
#include <queue>

namespace TTre
{
    /* Order of precedence: Highest to lowest - Associativity
      1) Parenthesis    - non-associative
      2) Kleene star    - left
      3) Concatenation  - left
      4) Union          - left
    */
    enum class OPERATOR_ORDER : unsigned
    {
        T_OPEN_PAREN = 4,
        T_CLOSE_PAREN = 4,
        T_KLEENE_STAR = 3,
        T_CHAR = 2, // concatenation
        T_UNION = 1
    };

    class Parser
    {
    public:
        // using enum OPERATOR_ORDER;

        Parser(Parser const&) = default;
        explicit Parser(std::string_view str) : source_(str), lexer_(str)
        {}
        ~Parser()
        {
            delete& lexer_;
        }
    public:
        std::string_view parse();
    private:
        void parse_kleene_star_();
        void parse_char_();
        void parse_union_();
        void parse_group_();
    private:
        std::string_view source_;
        Lexer lexer_;
        std::string output_{};
        std::string operators_{};
    };
}


#endif // TTRE_PARSER_H