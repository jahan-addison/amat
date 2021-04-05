#include <ttre/include/lexer.h>
#include <iostream>


int main()
{
    TTre::Tokens t = TTre::Tokens::_from_string("T_CHAR");
    std::cout << t;
}