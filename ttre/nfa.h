#pragma once

#include <set>
#include <list>
#include <utility>
#include <stack>

namespace ttre
{
    using Alphabet = std::set<unsigned char>;
    Alphabet const alphabet = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\x20', '}', '~',

        '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',

        ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`', '{', '|',

    };

    struct State
    {
        State(State const&) = default;
        State() = delete;

        enum class Type { initial, accept, normal };

        explicit State(unsigned short id_, Type type_) : id(id_), type(type_)
        {}

        inline friend bool operator<(State const& state1, State const& state2)
        {
            return state1.id < state2.id;
        }

        unsigned short id;
        Type type;

    };

    unsigned char const Epsilon = 0;

    template <class T = State>
    struct Edge
    {
        Edge() = delete;
        explicit Edge(unsigned char symbol_, std::pair<T, T>nodes_) : symbol(symbol_), nodes(nodes_)
        {}
        unsigned char symbol;
        std::pair<T, T> nodes;
    };

    struct NFA
    {
        using Input = unsigned char;

        explicit NFA(State start_) : start(start_)
        {
            states.insert(start_);
        }

        void connect(Input symbol, State& state);
        void connect(NFA& insert);

        State start;
        std::set<State> states;
        std::set<State> accepted;

        std::list<Edge<State>> edges;
    };

    using Automata = std::stack<NFA>;

    NFA construct_NFA_from_regular_expression(std::string_view source);
    NFA construct_NFA_from_character(unsigned char c, unsigned short start);
    NFA construct_NFA_from_concatenation(Automata& automaton);
} // namespace ttre