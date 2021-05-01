#include <ttre/ttre.h>
#include <iostream>

int main()
{
    std::string_view k = "abbc"; //"(j|k)*";
    // ttre::Lexer lexer{k};
    //ttre::Parser parser{k};
    // while (*lexer++ != +ttre::Token::T_END)
    // {
    //     std::cout << "Token: " << ttre::token_as_string[*lexer];
    //     std::cout << "\tValue: " << lexer.scanner().value_or(-1);
    //     std::cout << std::endl;
    // }
    //std::cout << parser.parse();
    ttre::NFA nfa = ttre::construct_NFA_from_regular_expression(k);
    std::cout << "Edges count: " << nfa.edges.size() << std::endl;
    for (auto& edge : nfa.edges)
    {
        std::cout << edge.nodes.first.id << " " << edge.symbol << " " << edge.nodes.second.id << " -> ";
    }
}