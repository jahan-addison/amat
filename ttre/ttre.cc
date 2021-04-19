#include <ttre/ttre.h>
#include <string_view>
#include <iostream>

int main()
{
    std::string_view k = "(j|k)*";
    TTre::Lexer lexer{k};

    while (*lexer++ != +TTre::Token::T_END)
    {
        std::cout << "Token: " << TTre::token_as_string[*lexer];
        std::cout << "\tValue: " << lexer.scanner().value_or(-1);
        std::cout << std::endl;
    }
}