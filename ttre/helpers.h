#pragma once

#include <string>
#include <ttre/nfa.h>

namespace ttre
{
    namespace util
    {
        std::string to_state_symbol(char k);
        void print_NFA(NFA const& nfa, std::string_view expr);
        void print_branch(NFA::Branch const& branch);
        void print_edges(NFA::Edges const& edges);
    }
} // namespace ttre