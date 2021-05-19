#pragma once

#include <ttre/helpers.h>
#include <ttre/tokens.h>
#include <ttre/lexer.h> 
#include <ttre/parser.h>
#include <ttre/nfa.h>

#include <set>
#include <algorithm>

namespace ttre
{
    namespace literals
    {
        /**
         * Type-safe regular expression string literal.
         */

        template<std::size_t N>
        struct Regular_Expression_String
        {
            char r[N]{};
            constexpr Regular_Expression_String(char const(&str)[N])
            {
                std::string_view str_ = str;
                auto regexp_view{str_
                    | std::views::transform([](char c) -> NFA::Input {
                        return static_cast<NFA::Input>(c);
                        })};

                std::ranges::copy(regexp_view, r);
            };
        };

        template<Regular_Expression_String RegExp>
        constexpr auto operator"" _r()
        {
            return RegExp.r;
        }
    } // namespace literals

    namespace util
    {
        /**
         * Utility functions to simulate an NFA by on-the-fly subset construction on ttre::NFA.
         */

        std::set<Edge::Node> epsilon_closure(NFA const& nfa, Edge::Node const& state);
        std::set<Edge::Node> epsilon_closure(NFA const& nfa, std::set<Edge::Node>& states);

        std::set<Edge::Node> transition(NFA const& nfa, std::set<Edge::Node> const& states, NFA::Input symbol);

    } // namespace util


    ///////////////////////////////
    // << core library functions >>
    ///////////////////////////////

    template<literals::Regular_Expression_String RegExp>
    void print()
    {
        constexpr auto content = RegExp.r;
        util::print_NFA(util::construct_NFA_from_regular_expression(content), content);
    }

    template<literals::Regular_Expression_String RegExp>
    bool match(std::string_view str)
    {
        constexpr auto content = RegExp.r;
        auto nfa = util::construct_NFA_from_regular_expression(content);
        auto step = util::epsilon_closure(nfa, nfa.start);

        for (auto const& c : str)
        {
            step = util::transition(nfa, step, c);
            step.merge(util::epsilon_closure(nfa, step));
        }
        util::print_states(step);
        return std::ranges::equal(step, nfa.accepted);
    }

} // namespace ttre
