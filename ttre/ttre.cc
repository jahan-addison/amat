#include <ttre/ttre.h>
#include <ranges>

#include <iostream>

namespace ttre
{
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
            step = util::epsilon_closure(nfa, step);
            step = util::transition(nfa, step, c);
        }

        return std::includes(step.begin(), step.end(),
            nfa.accepted.begin(), nfa.accepted.end());
    }

    namespace util
    {
        std::set<Edge::Node> epsilon_closure(NFA const& nfa, Edge::Node const& state)
        {
            std::set<Edge::Node> next{};
            std::ranges::for_each(nfa.edges,
                [&next, &state](NFA::Branch const& branch) {
                    auto state_found = false;
                    for (auto const& edge : branch)
                    {
                        if (state_found == true and edge.symbol != util::Epsilon)
                            state_found = false;

                        if (state_found == true and edge.symbol == util::Epsilon)
                        {
                            if (edge.nodes.first.get()->id < edge.nodes.second.get()->id)
                                next.emplace(edge.nodes.first);
                            if (edge.nodes.second != state)
                                next.emplace(edge.nodes.second);
                        }
                        if (edge.nodes.first == state or edge.nodes.second == state)
                        {
                            if (edge.nodes.first == state and edge.symbol == util::Epsilon)
                                next.emplace(edge.nodes.second);
                            state_found = true;
                        }
                    }
                });
            return next;
        }

        std::set<Edge::Node> epsilon_closure(NFA const& nfa, std::set<Edge::Node>& states)
        {
            std::set<Edge::Node> next{};
            std::ranges::for_each(states,
                [&nfa, &next](Edge::Node const& state) {
                    next.merge(epsilon_closure(nfa, state));
                });
            return next;
        }

        std::set<Edge::Node> transition(NFA const& nfa, std::set<Edge::Node> const& states, NFA::Input symbol)
        {
            std::set<Edge::Node> next{};
            std::ranges::for_each(nfa.edges,
                [&next, &states, &symbol](NFA::Branch const& branch) {
                    auto state_found = false;
                    std::ranges::for_each(branch,
                        [&next, &states, &symbol, &state_found](Edge const& edge) {
                            if (state_found == true)
                            {
                                next.emplace(edge.nodes.first);
                                if (edge.nodes.second.get()->type == State::Type::accept
                                    and edge.symbol == util::Epsilon)
                                {
                                    next.emplace(edge.nodes.second);
                                }
                                state_found = false;
                            }
                            if (states.contains(edge.nodes.first)
                                and edge.symbol == util::Epsilon)
                            {
                                next.emplace(edge.nodes.second);
                            }
                            if (states.contains(edge.nodes.first)
                                and symbol == edge.symbol
                                and state_found == false)
                            {
                                next.emplace(edge.nodes.second);
                            }
                            if ((states.contains(edge.nodes.first) or states.contains(edge.nodes.second))
                                and edge.symbol == symbol)
                            {
                                state_found = true;
                            }
                        });
                });
            return next;
        }

    } // namespace util

} // namespace ttre

// int main()
// {
//     ttre::print<"(abab)*">();
//     std::cout << std::boolalpha << ttre::match<"(abab)*">("abab");
// }
