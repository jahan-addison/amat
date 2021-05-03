#pragma once

#include <ttre/nfa.h>
#include <iostream>
#include <iomanip>

namespace ttre
{
    namespace util
    {
        std::string to_state_symbol(char k)
        {
            if (k == 0) return "ε";
            else return std::string{k};
        }

        void print(NFA const& nfa, std::string_view expr)
        {
            auto edges = nfa.edges;
            std::cout << "Regular Expression: " << std::quoted(expr) << std::endl;
            std::cout << "<<NFA>>" << std::endl;
            std::cout << "States: " << nfa.states.size() << std::endl;
            std::cout << "Initial state: " << nfa.start.id << std::endl;
            std::cout << "Final state: " << nfa.edges.back().back().nodes.second.id << std::endl;
            std::cout << "Branches: " << nfa.edges.size() << std::endl;
            for (auto const& branch : edges)
            {
                std::cout << "  Edges count: " << branch.size() << std::endl;
                for (auto const& edge : branch)
                {
                    std::cout << edge.nodes.first.id << " " << to_state_symbol(edge.symbol) << " " << edge.nodes.second.id;
                    if (!nfa.accepted.contains(edge.nodes.second))
                    {
                        std::cout << "  ->  ";
                    }
                }
                std::cout << std::endl;
            }
        }

    }
}