#include <ttre/helpers.h>
#include <ttre/nfa.h>
#include <string_view>
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

        void print_branch(NFA::Branch const& branch)
        {
            for (auto const& edge : branch)
            {
                std::cout << edge.nodes.first.get()->id << " " << to_state_symbol(edge.symbol) << " " << edge.nodes.second.get()->id;
                if (edge != branch.back())
                {
                    std::cout << "  ->  ";
                }
            }
        }
        void print_states(NFA const& nfa)
        {
            std::cout << "States: " << std::endl;
            for (auto const& state : nfa.states)
            {
                std::cout << "State: " << state.get()->id << " " << state.get()->type << std::endl;
            }
        }

        void print_states(std::set<Edge::Node> const& states)
        {
            std::cout << "States: " << std::endl;
            for (auto const& state : states)
            {
                std::cout << "State: " << state.get()->id << " " << state.get()->type << std::endl;
            }
        }

        void print_edges(NFA::Edges const& edges)
        {
            for (auto const& branch : edges)
            {
                std::cout << "  Edges count: " << branch.size() << std::endl;
                print_branch(branch);
                std::cout << std::endl;
            }
        }

        void print_NFA(NFA const& nfa, std::string_view expr)
        {
            auto edges = nfa.edges;
            std::cout << "Regular Expression: " << std::quoted(expr) << std::endl;
            std::cout << "<<NFA>>" << std::endl;
            std::cout << "States: " << nfa.states.size() << std::endl;
            std::cout << "Initial state: " << nfa.start.get()->id << std::endl;
            std::cout << "Final state: " << nfa.edges.back().back().nodes.second.get()->id << std::endl;
            std::cout << "Branches: " << nfa.edges.size() << std::endl;
            print_edges(edges);
        }

    }
}