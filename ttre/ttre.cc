#include <string_view>
#include <ttre/ttre.h>

int main()
{
    std::string_view k = "a|(a|(ab))*";
    // ttre::Lexer lexer{k};
    //ttre::Parser parser{k};
    // while (*lexer++ != +ttre::Token::T_END)
    // {
    //     std::cout << "Token: " << ttre::token_as_string[*lexer];
    //     std::cout << "\tValue: " << lexer.scanner().value_or(-1);
    //     std::cout << std::endl;
    // }
    // std::cout << parser.parse();
    ttre::NFA nfa = ttre::construct_NFA_from_regular_expression(k);
    ttre::util::print_NFA(nfa, k);
}