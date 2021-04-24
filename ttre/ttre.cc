#include <ttre/ttre.h>
#include <iostream>

int main()
{
    std::string_view k = "(j|k)*";
    // ttre::Lexer lexer{k};
    ttre::Parser parser{k};
    // while (*lexer++ != +ttre::Token::T_END)
    // {
    //     std::cout << "Token: " << ttre::token_as_string[*lexer];
    //     std::cout << "\tValue: " << lexer.scanner().value_or(-1);
    //     std::cout << std::endl;
    // }
    std::cout << parser.parse();
}