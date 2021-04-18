#include <ttre/ttre.h>
#include <string_view>
#include <iostream>

int main()
{
    TTre::Token::_enumerated token;
    std::string_view k = "j|k*";
    TTre::Lexer lexer{k};

    while ((token = lexer.get_next_token()) != TTre::Token::T_END)
    {
        std::cout << TTre::token_as_string[token] << ", ";
    }
}