#include <ttre/parser.h>
#include <ttre/nfa.h>

#include <string_view>
#include <stdexcept>

namespace ttre
{

    void NFA::connect(Input symbol, State& state)
    {
        states.insert(state);
        if (state.type == State::Type::accept)
        {
            accepted.insert(state);
        }
        if (edges.empty())
        {
            Edge edge{symbol, {this->start, state}};
            this->edges.push_back(edge);

        }
        else
        {
            auto head = this->edges.front().nodes.second;
            Edge edge{symbol, {head, state}};
            this->edges.push_back(edge);
        }
    }

    void NFA::connect(NFA& insert)
    {
        auto first = this->edges.front();
        auto last = this->edges.back();
        auto last_i = insert.edges.back();

        first.nodes.first.type = State::Type::initial;
        if (last.nodes.second.type == State::Type::accept)
        {
            last.nodes.second.type = State::Type::normal;
        }
        last_i.nodes.first.type = State::Type::normal;
        last_i.nodes.second.type = State::Type::accept;
        this->start = first.nodes.first;
        this->states.merge(insert.states);
        this->accepted = {last_i.nodes.second};
        this->edges.splice(this->edges.begin(), insert.edges);
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
            case '.':
                automata.push(construct_NFA_from_concatenation(automata));
                break;
            default:
                unsigned short last_id = 0;
                if (automata.size())
                {
                    last_id = automata.top().edges.back().nodes.second.id + 1;
                }
                automata.push(construct_NFA_from_character(item, last_id));
                break;
            }
        }

        nfa.connect(automata.top());
        automata.pop();
        return nfa;
    }

    NFA construct_NFA_from_concatenation(Automata& automata)
    {
        if (automata.size() < 2)
        {
            std::runtime_error("could not constract NFA from concat operator and the stack");
        }

        NFA arg1 = automata.top();
        automata.pop();
        NFA arg2 = automata.top();
        automata.pop();
        arg1.connect(arg2);
        return arg1;
    }

    NFA construct_NFA_from_character(unsigned char c, unsigned short start)
    {
        State start_state{start, State::Type::initial};
        State end_state{++start, State::Type::accept};
        NFA nfa{start_state};
        nfa.connect(c, end_state);
        return nfa;
    }

} // namespace ttre