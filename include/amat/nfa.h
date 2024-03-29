#pragma once

#include <algorithm>
#include <array>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <stack>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

#include <amat/helpers.h>
#include <amat/nfa.h>
#include <amat/parser.h>

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

  public:
    void connect_edge(Input symbol, Edge::Node& state, size_t location)
    {
        this->states.emplace(state);
        if (edges.empty()) {
            Edge edge{ symbol, { this->start, state } };
            this->edges.push_back({ edge });
        } else {
            if (this->edges.size() < location) {
                Branch branch = this->edges[location];
                Edge::Node head = branch.back().nodes.second;
                Edge edge{ symbol, { head, state } };
                branch.push_back(edge);
            }
        }
    }

    void connect_branch(Branch& to, Branch& from)
    {
        auto first = from.front();
        auto last = from.back();
        [[maybe_unused]] auto last_i = to.front();

        first.nodes.first.get()->type = State::Type::initial;
        if (last.nodes.second.get()->type == State::Type::accept) {
            last.nodes.second.get()->type = State::Type::normal;
        }
        last_i.nodes.first.get()->type = State::Type::normal;
        last_i.nodes.second.get()->type = State::Type::accept;

        this->start = first.nodes.first;

        from.splice(from.begin(), to);
    }

    void connect_NFA(NFA& nfa)
    {
        std::ranges::copy(
          nfa.edges.begin(), nfa.edges.end(), std::back_inserter(this->edges));
        auto front = this->edges.front().front();
        auto back = this->edges.back().back();
        std::ranges::for_each(
          nfa.edges, [=, this, &front, &back](NFA::Branch& branch) {
              std::ranges::for_each(
                branch.begin(),
                branch.end(),
                [=, this, &front, &back](auto& edge) {
                    edge.nodes.first.get()->type =
                      edge.nodes.second.get()->type = State::Type::normal;
                    if (front.nodes.first != edge.nodes.first)
                        states.emplace(edge.nodes.first);
                    if (front.nodes.first != edge.nodes.second)
                        states.emplace(edge.nodes.second);
                });
          });
        front.nodes.first.get()->type = State::Type::initial;
        this->start = front.nodes.first;
        back.nodes.second.get()->type = State::Type::accept;
        this->accepted = { back.nodes.second };
    }

  public:
    Edge::Node start;

    std::set<Edge::Node> states{};
    std::set<Edge::Node> accepted{};

    Edges edges;
};

namespace util {

// forward declarations
NFA construct_NFA_from_regular_expression(std::string_view);
NFA
construct_NFA_from_character(unsigned char, unsigned short);
NFA
construct_NFA_from_concatenation(Automata&);
NFA
construct_NFA_from_kleene_star(Automata&);
NFA
construct_NFA_from_union(Automata&);

NFA
construct_NFA_from_regular_expression(std::string_view source)
{
    Parser parser{ source };
    auto parsed = parser.parse();
    Edge::Node empty =
      std::make_shared<State>(State{ 0, State::Type::initial });
    Automata automata{};
    auto nfa = NFA{ empty };
    for (auto const& item : parsed) {
        switch (item) {
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
                if (automata.size()) {
                    last_id = automata.top()
                                .edges.back()
                                .back()
                                .nodes.second.get()
                                ->id +
                              1;
                }
                automata.push(construct_NFA_from_character(item, last_id));
                break;
        }
    }
    nfa.connect_NFA(automata.top());

    automata.pop();
    return nfa;
}

namespace {

// forward declaration
void
reevaluate_each_state_id_on_branch(NFA::Branch& branch, unsigned short* start);

void
prepend_start_transition_each_branch(NFA& nfa,
                                     Edge::Node const& state,
                                     unsigned short* start_id)
{
    auto i = 0;
    std::ranges::for_each(
      nfa.edges, [&state, &start_id, &i](NFA::Branch& branch) {
          branch.front().nodes.first.get()->id = 1;
          if (i > 0) {
              branch.front().nodes.second.get()->id = ++(*start_id);
          }
          Edge start_edge{ Epsilon, { state, branch.front().nodes.first } };
          branch.push_front(start_edge);
          reevaluate_each_state_id_on_branch(branch, start_id);
          i++;
      });
}

void
append_end_transition_each_branch(NFA& nfa, Edge::Node const& state)
{
    nfa.accepted.emplace(state);
    std::ranges::for_each(nfa.edges, [&state](NFA::Branch& branch) {
        Edge::Node prev =
          std::make_shared<State>(State{ state.get()->id, state.get()->type });
        prev.get()->id = branch.back().nodes.second.get()->id + 1;
        Edge end_edge{ Epsilon, { prev, state } };
        branch.push_back(end_edge);
    });
}

void
reevaluate_each_state_id_on_branch(NFA::Branch& branch, unsigned short* start)
{
    std::ranges::for_each(
      std::next(branch.begin()), branch.end(), [&start, &branch](Edge& edge) {
          edge.nodes.first.get()->type = State::Type::normal;
          edge.nodes.second.get()->type = State::Type::normal;
          if (edge.nodes.second.get()->id > edge.nodes.first.get()->id or
              edge.symbol != Epsilon) {
              edge.nodes.first.get()->id = (*start)++;
              edge.nodes.second.get()->id = (*start)++;
          } else {
              auto first = *std::next(branch.begin());
              edge.nodes.first.get()->id = *start;
              edge.nodes.second.get()->id = first.nodes.second.get()->id + 1;
          }
      });
}

void
connect_kleene_edge_on_single_branch(Automata& automata,
                                     Edge::Node& start_state,
                                     Edge& cyclic_forward_edge,
                                     Edge& cyclic_edge,
                                     NFA::Branch& back,
                                     NFA::Branch& forward)
{
    if (automata.size() >= 1)
        cyclic_forward_edge.nodes.first =
          automata.top().edges.back().back().nodes.second;
    else {
        cyclic_forward_edge.nodes.first = start_state;
        back.back().nodes.first->id++;
    }
    cyclic_forward_edge.nodes.second = forward.front().nodes.first;
    back.back().nodes.second = forward.back().nodes.first;
    cyclic_edge.nodes.first = back.back().nodes.second;
    cyclic_edge.nodes.second = cyclic_forward_edge.nodes.first;
    back.push_front(cyclic_forward_edge);
    back.push_back(cyclic_edge);
    forward.splice(forward.begin(), back);
}

void
connect_kleene_edge_multiple_branches(NFA& nfa,
                                      Edge& cyclic_forward_edge,
                                      Edge& cyclic_edge,
                                      NFA::Branch& back,
                                      NFA::Branch& forward)
{
    cyclic_forward_edge.nodes.first = back.back().nodes.second;
    cyclic_forward_edge.nodes.second = nfa.edges.back().back().nodes.second;
    cyclic_edge.nodes.first = forward.back().nodes.second;
    forward.push_back(cyclic_edge);
    forward.push_front(cyclic_forward_edge);
    forward.splice(forward.begin(), back);
}

} // namespace

NFA
construct_NFA_from_character(unsigned char c, unsigned short start)
{
    Edge::Node start_state =
      std::make_shared<State>(State{ start, State::Type::initial });
    Edge::Node end_state =
      std::make_shared<State>(State{ ++start, State::Type::accept });
    NFA nfa{ start_state };
    if (!alphabet.contains(c)) {
        std::cerr << "Invalid character: \"" << c << "\"" << std::endl;
        throw std::runtime_error("operator not defined in alphabet");
    }
    nfa.connect_edge(c, end_state, 0);
    return nfa;
}

NFA
construct_NFA_from_concatenation(Automata& automata)
{
    if (automata.size() < 2) {
        return automata.top();
    }
    NFA arg1 = automata.top();
    automata.pop();
    NFA arg2 = automata.top();
    automata.pop();
    arg1.connect_branch(arg2.edges.back(), arg1.edges.back());
    return arg1;
}

NFA
construct_NFA_from_kleene_star(Automata& automata)
{
    Edge::Node start_state =
      std::make_shared<State>(State{ 0, State::Type::initial });
    Edge::Node end_state =
      std::make_shared<State>(State{ 1, State::Type::accept });
    NFA nfa{ start_state };
    if (automata.size() < 1) {
        throw std::runtime_error(
          "could not constract NFA from kleene star operator and the stack");
    }

    NFA arg = automata.top();
    std::optional<NFA> arg2{};

    automata.pop();

    if (automata.size() >= 1) {
        arg2 = std::make_optional<NFA>(automata.top());
        automata.pop();
    } else {
        auto forward = arg.edges.back().front().nodes.first;
        Edge forward_edge{ Epsilon,
                           { arg.edges.back().front().nodes.first,
                             arg.edges.back().back().nodes.second } };
        arg.edges.back().push_front(forward_edge);
    }

    auto last_edge = arg.edges.back().back();

    std::ranges::for_each(
      arg.edges,
      [&arg, &arg2, &end_state, &start_state, &automata](NFA::Branch& branch) {
          auto back = arg2.has_value()
                        ? arg2.value().edges.back().back().nodes.second
                        : branch.front().nodes.first;
          auto front = arg2.has_value()
                         ? arg2.value().edges.back().back().nodes.second
                         : branch.back().nodes.second;

          Edge cyclic_forward_edge{ Epsilon, { back, end_state } };

          Edge cyclic_edge{ Epsilon, { front, back } };

          if (arg2.has_value()) {
              if (arg2->edges.back().size() == 1)
                  connect_kleene_edge_on_single_branch(
                    automata,
                    start_state,
                    cyclic_forward_edge,
                    cyclic_edge,
                    arg2.value().edges.back(),
                    branch);
              else
                  connect_kleene_edge_multiple_branches(
                    arg,
                    cyclic_forward_edge,
                    cyclic_edge,
                    arg2.value().edges.back(),
                    branch);
          } else {
              branch.push_back(cyclic_edge);
          }
      });

    end_state.get()->id = arg.edges.back().back().nodes.first.get()->id + 2;

    if (automata.empty())
        append_end_transition_each_branch(arg, end_state);

    nfa.connect_NFA(arg);

    return nfa;
}

NFA
construct_NFA_from_union(Automata& automata)
{
    Edge::Node start_state =
      std::make_shared<State>(State{ 0, State::Type::initial });
    Edge::Node end_state =
      std::make_shared<State>(State{ 0, State::Type::accept });
    NFA nfa{ start_state };
    if (automata.size() < 2) {
        throw std::runtime_error(
          "could not constract NFA from union operator and the stack");
    }
    NFA arg1 = automata.top();
    automata.pop();
    NFA arg2 = automata.top();
    automata.pop();

    std::array<NFA*, 2> root_branches = { &arg2, &arg1 };

    unsigned short id = start_state.get()->id + 2;

    std::ranges::for_each(root_branches.begin(),
                          root_branches.end(),
                          [&id, &start_state](NFA*& arg) {
                              prepend_start_transition_each_branch(
                                *arg, start_state, &id);
                              id++;
                          });

    end_state.get()->id = id;

    std::ranges::for_each(
      root_branches.begin(), root_branches.end(), [&id, &end_state](NFA*& arg) {
          id++;
          append_end_transition_each_branch(*arg, end_state);
      });

    nfa.connect_NFA(arg2);
    nfa.connect_NFA(arg1);

    return nfa;
}

} // namespace util

} // namespace amat
