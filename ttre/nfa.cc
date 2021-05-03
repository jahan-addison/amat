#include <ttre/parser.h>
#include <ttre/nfa.h>
#include <algorithm>
#include <array>
#include <string_view>
#include <iostream>
#include <stdexcept>

namespace ttre
{
    void reevaluate_each_state_id_on_branch(NFA::Branch& branch, unsigned short* start);

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
        std::ranges::for_each(nfa.edges,
            [=, this](NFA::Branch& branch) {
                std::ranges::for_each(branch.begin(), branch.end(),
                    [=, this](auto& edge) {
                        states.insert(edge.nodes.first);
                        states.insert(edge.nodes.second);
                    });
            });
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
                automata.push(construct_NFA_from_union(automata));
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

    void prepend_start_transition_each_branch(NFA& nfa, State const& state, unsigned short* start_id)
    {
        auto i = 0;
        std::ranges::for_each(nfa.edges,
            [&state, &start_id, &i](NFA::Branch& branch)
            {
                branch.front().nodes.first.id = 1;
                if (i > 0)
                {
                    branch.front().nodes.second.id = ++(*start_id);
                }
                Edge start_edge{Epsilon, {state, branch.front().nodes.first}};
                branch.push_front(start_edge);
                reevaluate_each_state_id_on_branch(branch, start_id);
                i++;
            });
    }


    void append_end_transition_each_branch(NFA& nfa, State const& state)
    {
        nfa.accepted.insert(state);
        std::ranges::for_each(nfa.edges,
            [&state](NFA::Branch& branch)
            {
                State prev{state};
                prev.id = branch.back().nodes.second.id + 1;
                Edge end_edge{Epsilon, {prev, state}};
                branch.push_back(end_edge);
            });
    }

    void reevaluate_each_state_id_on_branch(NFA::Branch& branch, unsigned short* start)
    {
        std::ranges::for_each(std::next(branch.begin()), branch.end(),
            [&start](auto& edge) {
                edge.nodes.first.id = (*start)++;
                edge.nodes.second.id = (*start)++;
            });
    }


    NFA construct_NFA_from_union(Automata& automata)
    {
        State start_state{0, State::Type::initial};
        State end_state{0, State::Type::accept};
        NFA nfa{start_state};
        if (automata.size() < 2)
        {
            throw std::runtime_error("could not constract NFA from union operator and the stack");
        }
        NFA arg1 = automata.top();
        automata.pop();
        NFA arg2 = automata.top();
        automata.pop();

        std::array<NFA*, 2> root_branches = {&arg2, &arg1};

        unsigned short id = start_state.id + 2;

        std::ranges::for_each(root_branches.begin(), root_branches.end(),
            [&id, &start_state](NFA*& arg) {
                prepend_start_transition_each_branch(*arg, start_state, &id);
                id++;
            });

        end_state.id = id;

        std::ranges::for_each(root_branches.begin(), root_branches.end(),
            [&id, &end_state](NFA*& arg) {
                id++;
                append_end_transition_each_branch(*arg, end_state);
            });

        nfa.connect_NFA(arg2);
        nfa.connect_NFA(arg1);

        return nfa;
    }

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