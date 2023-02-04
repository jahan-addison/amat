#pragma once

#include <amat/helpers.h>
#include <amat/lexer.h>
#include <amat/nfa.h>
#include <amat/parser.h>
#include <amat/tokens.h>
#include <iostream>

#include <algorithm>
#include <ranges>
#include <set>

namespace amat {
namespace literals {
/**
 * Type-safe regular expression string literal.
 */

template<std::size_t N>
struct Regular_Expression_String
{
    char r[N]{};
    constexpr Regular_Expression_String(char const (&str)[N])
    {
        std::string_view str_ = str;
        auto regexp_view{ str_ |
                          std::views::transform([](char c) -> NFA::Input {
                              return static_cast<NFA::Input>(c);
                          }) };

        std::ranges::copy(regexp_view, r);
    };
};

template<Regular_Expression_String RegExp>
constexpr auto operator"" _r()
{
    return RegExp.r;
}
} // namespace literals

namespace util {
using States = std::set<Edge::Node>;

/**
 * Utilities to simulate an NFA by on-the-fly subset construction on amat::NFA.
 */

struct Simulator
{
    Simulator() = delete;
    Simulator(NFA nfa_)
      : nfa(nfa_)
    {
        old_states = nfa_.states;
    }
    void add_state(Edge::Node state);
    void get_next_closed_transition(NFA::Input c);

    NFA nfa;
    States old_states{};
    States new_states{};
    std::vector<Edge::Node> on{};
};

States
epsilon_closure(NFA const& nfa, Edge::Node const& state);
States
epsilon_closure(NFA const& nfa, States& states);
States
transition(NFA const& nfa, States const& states, NFA::Input symbol);

} // namespace util

///////////////////////////////
// << core library functions >>
///////////////////////////////

template<literals::Regular_Expression_String RegExp>
void
print()
{
    constexpr auto content = RegExp.r;
    util::print_NFA(util::construct_NFA_from_regular_expression(content),
                    content);
}

template<literals::Regular_Expression_String RegExp>
bool
match(std::string_view str)
{
    constexpr auto content = RegExp.r;
    auto nfa = util::construct_NFA_from_regular_expression(content);
    util::Simulator dfa{ nfa };
    dfa.add_state(nfa.start);
    for (auto const& c : str) {
        dfa.get_next_closed_transition(c);
    }

    return std::ranges::equal(dfa.old_states, nfa.accepted);
}

} // namespace amat
