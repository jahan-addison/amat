#include <ttre/parser.h>
#include <ttre/nfa.h>
#include <ttre/helpers.h>
#include <algorithm>
#include <array>
#include <string_view>
#include <iostream>
#include <stdexcept>

namespace ttre
{
    void NFA::connect_edge(Input symbol, Edge::Node& state, size_t location)
    {
        this->states.emplace(state);
        if (state.get()->type == State::Type::accept)
        {
            accepted.emplace(state);
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
                Edge::Node head = branch.back().nodes.second;
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

        first.nodes.first.get()->type = State::Type::initial;
        if (last.nodes.second.get()->type == State::Type::accept)
        {
            last.nodes.second.get()->type = State::Type::normal;
        }
        last_i.nodes.first.get()->type = State::Type::normal;
        last_i.nodes.second.get()->type = State::Type::accept;
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
            [=, this, &back](NFA::Branch& branch) {
                std::ranges::for_each(branch.begin(), branch.end(),
                    [=, this, &back](auto& edge) {
                        edge.nodes.first.get()->type = edge.nodes.second.get()->type = State::Type::normal;
                        states.emplace(edge.nodes.first);
                        states.emplace(edge.nodes.second);
                    });
            });
        front.nodes.first.get()->type = State::Type::initial;
        this->start = front.nodes.first;
        this->states.erase(std::prev(this->states.end()));
        this->states.emplace(back.nodes.second);
        back.nodes.second.get()->type = State::Type::accept;
        this->accepted = {back.nodes.second};
    }

    namespace util
    {
        NFA construct_NFA_from_regular_expression(std::string_view source)
        {
            Parser parser{source};
            auto parsed = parser.parse();
            Edge::Node empty = std::make_shared<State>(State{0, State::Type::initial});
            Automata automata{};
            auto nfa = NFA{empty};
            for (auto const& item : parsed)
            {
                switch (item)
                {
                case '|':
                    automata.push(construct_NFA_from_union(automata));
                    break;
                case '*':
                    automata.push(construct_NFA_from_kleene_star(automata));
                    break;
                case '.':
                    automata.push(construct_NFA_from_concatenation(automata));
                    break;
                default:
                    unsigned short last_id = 0;
                    if (automata.size())
                    {
                        last_id = automata.top().edges.back().back().nodes.second.get()->id + 1;
                    }
                    automata.push(construct_NFA_from_character(item, last_id));
                    break;
                }
            }
            nfa.connect_NFA(automata.top());

            automata.pop();
            return nfa;
        }

        void prepend_start_transition_each_branch(NFA& nfa, Edge::Node const& state, unsigned short* start_id)
        {
            auto i = 0;
            std::ranges::for_each(nfa.edges,
                [&state, &start_id, &i](NFA::Branch& branch)
                {
                    branch.front().nodes.first.get()->id = 1;
                    if (i > 0)
                    {
                        branch.front().nodes.second.get()->id = ++(*start_id);
                    }
                    Edge start_edge{Epsilon, {state, branch.front().nodes.first}};
                    branch.push_front(start_edge);
                    reevaluate_each_state_id_on_branch(branch, start_id);
                    i++;
                });
        }


        void append_end_transition_each_branch(NFA& nfa, Edge::Node const& state)
        {
            nfa.accepted.emplace(state);
            std::ranges::for_each(nfa.edges,
                [&state](NFA::Branch& branch)
                {
                    Edge::Node prev = std::make_shared<State>(State{state.get()->id, state.get()->type});
                    prev.get()->id = branch.back().nodes.second.get()->id + 1;
                    Edge end_edge{Epsilon, {prev, state}};
                    branch.push_back(end_edge);
                });
        }

        void reevaluate_each_state_id_on_branch(NFA::Branch& branch, unsigned short* start)
        {
            std::ranges::for_each(std::next(branch.begin()), branch.end(),
                [&start, &branch](Edge& edge) {
                    edge.nodes.first.get()->type = State::Type::normal;
                    edge.nodes.second.get()->type = State::Type::normal;
                    if (edge.nodes.second.get()->id > edge.nodes.first.get()->id
                        or edge.symbol != Epsilon)
                    {
                        edge.nodes.first.get()->id = (*start)++;
                        edge.nodes.second.get()->id = (*start)++;

                    }
                    else
                    {
                        auto first = *std::next(branch.begin());
                        edge.nodes.first.get()->id = (*start);
                        edge.nodes.second.get()->id = first.nodes.second.get()->id + 1;

                    }
                });
        }

        NFA construct_NFA_from_kleene_star(Automata& automata)
        {
            Edge::Node start_state = std::make_shared<State>(State{0, State::Type::initial});
            Edge::Node end_state = std::make_shared<State>(State{0, State::Type::accept});
            NFA nfa{start_state};
            if (automata.size() < 1)
            {
                throw std::runtime_error("could not constract NFA from kleene star operator and the stack");

            }

            NFA arg = automata.top();
            automata.pop();

            bool is_single_branch = arg.edges.size() == 1 and automata.size() >= 1;
            auto cyclic_forward = 0;
            std::ranges::for_each(arg.edges,
                [&cyclic_forward, &end_state, &is_single_branch, &automata](NFA::Branch& branch) {
                    auto front = branch.back().nodes.second;
                    front.get()->id++;
                    auto back = is_single_branch ?
                        automata.top().edges.front().back().nodes.first
                        : branch.front().symbol == 0 ?
                        (*std::next(branch.begin(), 1)).nodes.first
                        : branch.front().nodes.first;
                    if (is_single_branch)
                    {
                        back.get()->id += 2;
                    }
                    cyclic_forward = front.get()->id + 1;
                    Edge cyclic_edge{Epsilon,
                        {front, back}};

                    branch.push_back(cyclic_edge);

                });

            end_state.get()->id = arg.edges.back().back().nodes.first.get()->id + 2;

            append_end_transition_each_branch(arg, end_state);

            arg.edges.back().back().nodes.first.get()->id = cyclic_forward;

            if (automata.size() > 0)
            {
                NFA::Branch forward_branch = automata.top().edges.front();
                Edge forward_edge{Epsilon, {forward_branch.back().nodes.second, end_state}};
                forward_edge.nodes.first.get()->id++;

                forward_branch.push_back(forward_edge);

                nfa.edges.push_back(forward_branch);
            }
            else
            {
                Edge forward_edge{Epsilon, {arg.edges.front().front().nodes.first, end_state}};

                nfa.edges.push_back({forward_edge});
                automata.push(arg);

            }

            nfa.connect_NFA(arg);
            return nfa;
        }


        NFA construct_NFA_from_union(Automata& automata)
        {
            Edge::Node start_state = std::make_shared<State>(State{0, State::Type::initial});
            Edge::Node end_state = std::make_shared<State>(State{0, State::Type::accept});
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

            unsigned short id = start_state.get()->id + 2;

            std::ranges::for_each(root_branches.begin(), root_branches.end(),
                [&id, &start_state](NFA*& arg) {
                    prepend_start_transition_each_branch(*arg, start_state, &id);
                    id++;
                });

            end_state.get()->id = id;

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
            Edge::Node start_state = std::make_shared<State>(State{start, State::Type::initial});
            Edge::Node end_state = std::make_shared<State>(State{++start, State::Type::accept});
            NFA nfa{start_state};
            if (!alphabet.contains(c))
            {
                std::cerr << "Invalid character: \"" << c << "\"" << std::endl;
                throw std::runtime_error("operator not defined in alphabet");
            }
            nfa.connect_edge(c, end_state, 0);
            return nfa;
        }
    } // namespace util

} // namespace ttre