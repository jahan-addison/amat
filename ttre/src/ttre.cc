#include <ttre/include/lexer.h>
#include <iostream>


int main()
{
    Ttre::Tokens t = Ttre::Tokens::_from_string("T_CHAR");
    std::cout << t;
}