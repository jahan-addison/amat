#include <ttre/parser.h>
#include <ttre/nfa.h>
#include <algorithm>
#include <string_view>
#include <iostream>
#include <stdexcept>

namespace ttre
{

    void NFA::connect_edge(Input symbol, State& state, size_t location)
    {
        states.insert(state);
        if (state.type == State::Type::accept)
        {
            accepted.insert(state);
        }
        if (edges.empty())
        {
            Edge edge{symbol, {this->start, state}};
            this->edges.push_back({edge});
        }
        else
        {
            if (this->edges.size() < location)
            {
                Branch branch = this->edges[location];
                State head = branch.back().nodes.second;
                Edge edge{symbol, {head, state}};
                branch.push_back(edge);

            }
        }
    }

    void NFA::connect_branch(Branch& to, Branch& from)
    {
        auto first = from.front();
        auto last = from.back();
        [[maybe_unused]] auto last_i = to.front();

        first.nodes.first.type = State::Type::initial;
        if (last.nodes.second.type == State::Type::accept)
        {
            last.nodes.second.type = State::Type::normal;
        }
        last_i.nodes.first.type = State::Type::normal;
        last_i.nodes.second.type = State::Type::accept;
        this->start = first.nodes.first;

        from.splice(from.begin(), to);
    }

    void NFA::connect_NFA(NFA& nfa)
    {
        std::ranges::copy(nfa.edges.begin(), nfa.edges.end(),
            std::back_inserter(this->edges));
        auto front = this->edges.front().front();
        auto back = this->edges.back().back();
        this->states.merge(nfa.states);
        this->start = front.nodes.first;
        this->accepted = {back.nodes.second};
    }


    NFA construct_NFA_from_regular_expression(std::string_view source)
    {
        Parser parser{source};
        auto parsed = parser.parse();
        State empty{0, State::Type::initial};
        Automata automata{};
        auto nfa = NFA{empty};
        for (auto const& item : parsed)
        {
            switch (item)
            {
            case '|':
                break;
            case '.':
                automata.push(construct_NFA_from_concatenation(automata));
                break;
            default:
                unsigned short last_id = 0;
                if (automata.size())
                {
                    last_id = automata.top().edges.back().back().nodes.second.id + 1;
                }
                automata.push(construct_NFA_from_character(item, last_id));
                nfa.states.merge(automata.top().states);
                break;
            }
        }
        nfa.connect_NFA(automata.top());
        automata.pop();
        return nfa;
    }

    // NFA construct_NFA_from_union(Automata& automata)
    // {

    // }

    NFA construct_NFA_from_concatenation(Automata& automata)
    {
        if (automata.size() < 2)
        {
            throw std::runtime_error("could not constract NFA from concat operator and the stack");
        }
        NFA arg1 = automata.top();
        automata.pop();
        NFA arg2 = automata.top();
        automata.pop();
        arg1.connect_branch(arg2.edges.front(), arg1.edges.back());
        return arg1;
    }

    NFA construct_NFA_from_character(unsigned char c, unsigned short start)
    {
        State start_state{start, State::Type::initial};
        State end_state{++start, State::Type::accept};
        NFA nfa{start_state};
        if (!alphabet.contains(c))
        {
            std::cerr << "Invalid character: \"" << c << "\"" << std::endl;
            throw std::runtime_error("operator not defined in alphabet");
        }
        nfa.connect_edge(c, end_state, 0);
        return nfa;
    }

} // namespace ttre