#pragma once

#include <list>
#include <memory>
#include <ostream>
#include <set>
#include <stack>
#include <utility>
#include <vector>

namespace amat {

struct NFA;

using Automata = std::stack<NFA>;

namespace util {
using Alphabet = std::set<unsigned char>;

Alphabet const alphabet = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G',  'H', 'I', 'J', 'K',    'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T',  'U', 'V', 'W', 'X',    'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g',  'h', 'i', 'j', 'k',    'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't',  'u', 'v', 'w', 'x',    'y', 'z',

    '0', '1', '2', '3', '4', '5', '6',  '7', '8', '9', '\x20', '}', '~',

    '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',    ',', '-',
    '.', '/', ':', ';', '<', '=', '>',  '?', '@', '[', '\\',   ']', '^',
    '_', '`', '{', '|',

};

unsigned char const Epsilon = 0;

} // namespace util

struct State
{
    State(State const&) = default;
    State() = delete;

    enum class Type
    {
        initial,
        accept,
        normal
    };

    friend constexpr std::ostream& operator<<(std::ostream& os, Type type)
    {
        switch (type) {
            case State::Type::initial:
                return os << 0;
            case State::Type::normal:
                return os << 1;
            case State::Type::accept:
                return os << 2;
            default:
                return os << -1;
        }
    }

    explicit State(unsigned short id_, Type type_)
      : id(id_)
      , type(type_)
    {
    }

    inline friend bool operator==(State const& left, State const& right)
    {
        return &left == &right and left.id == right.id;
    }

    inline friend bool operator!=(State const& left, State const& right)
    {
        return !operator==(left, right);
    }

    auto operator<=>(State const&) const = default;

    inline friend bool operator<(State const& state1, State const& state2)
    {
        return state1.id < state2.id;
    }

    unsigned short id;
    Type type;
};

struct Edge
{
    using Node = std::shared_ptr<State>;

    Edge() = delete;

    explicit Edge(unsigned char symbol_, std::pair<Node, Node> nodes_)
      : symbol(symbol_)
      , nodes(nodes_)
    {
    }

    inline friend int operator==(Edge const& left, Edge const& right)
    {
        return &left == &right;
    }
    inline friend int operator!=(Edge const& left, Edge const& right)
    {
        return !operator==(left, right);
    }

    unsigned char symbol;

    std::pair<Node, Node> nodes;
};

/**
 * NFA represented as an adjacency-list graph.
 */
struct NFA
{
    using Input = unsigned char;
    using Branch = std::list<Edge>;
    using Edges = std::vector<Branch>;

    NFA(NFA const&) = default;

    explicit NFA(Edge::Node start_)
      : start(start_)
    {
        states.emplace(start_);
    }
    void connect_edge(Input symbol, Edge::Node& state, size_t location);
    void connect_branch(Branch& to, Branch& from);
    void connect_NFA(NFA& nfa);

    Edge::Node start;

    std::set<Edge::Node> states{};
    std::set<Edge::Node> accepted{};

    Edges edges;
};

namespace util {
NFA construct_NFA_from_regular_expression(std::string_view);
NFA
construct_NFA_from_character(unsigned char, unsigned short);
NFA
construct_NFA_from_concatenation(Automata&);
NFA
construct_NFA_from_kleene_star(Automata&);
NFA
construct_NFA_from_union(Automata&);

void
reevaluate_each_state_id_on_branch(NFA::Branch& branch, unsigned short* start);

} // namespace util

} // namespace amat
