#pragma once

#include <ttre/nfa.h>
#include <iostream>

namespace ttre
{
    namespace util
    {
        void print(NFA const& nfa)
        {
            auto edges = nfa.edges;
            std::cout << "<<NFA>>" << std::endl;
            std::cout << "States: " << nfa.states.size() << std::endl;
            std::cout << "Initial state: " << nfa.start.id << " \"" << nfa.edges.front().front().symbol << "\"" << std::endl;
            std::cout << "Final state: " << nfa.edges.back().back().nodes.second.id << " \"" << nfa.edges.back().back().symbol << "\"" << std::endl;
            std::cout << "Branches: " << nfa.edges.size() << std::endl;
            for (auto const& branch : edges)
            {
                std::cout << "  Edges count: " << branch.size() << std::endl;
                for (auto const& edge : branch)
                {
                    std::cout << "  " << edge.nodes.first.id << " " << edge.symbol << " " << edge.nodes.second.id << " -> ";
                }
                std::cout << std::endl;
            }
        }

    }
}