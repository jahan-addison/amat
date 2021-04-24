#pragma once

#include <set>
#include <tuple>
#include <utility>

namespace ttre
{
    std::set<unsigned char> const Alphabet = {
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
        explicit State(unsigned short id_) : id(id_), type(Type::normal)
        {}

        enum class Type { initial, accept, normal };

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
        unsigned char symbol;
        std::pair<T, T> nodes;
    };

    struct State_Transition_Graph
    {
        using Input = unsigned char;

        State start;
        std::set<State> states;
        std::set<State> accepted;

        virtual std::set<State> transition(State s, Input symbol) = 0;

        Edge<State> edges;
    };

    struct NFA : public State_Transition_Graph
    {
        std::set<State> transition(State s, unsigned char symbol);
    };

} // namespace ttre