#include <ttre/ttre.h>
#include <iostream>
#include <ranges>

namespace ttre
{
    using namespace util;

    template<literals::Regular_Expression_String RegExp>
    void print()
    {
        constexpr auto content = RegExp.r;
        print_NFA(construct_NFA_from_regular_expression(content), content);
    }

    template<literals::Regular_Expression_String RegExp>
    bool match(std::string_view str)
    {
        constexpr auto content = RegExp.r;
        auto nfa = construct_NFA_from_regular_expression(content);
        auto step = epsilon_closure(nfa, nfa.start);
        std::set<State> final{};
        for (auto const& c : str)
        {
            step = transition(nfa, step, c);
            final = epsilon_closure(nfa, step);
            // for (auto const& state : step)
            // {
            //     std::cout << state.id << " ";
            // }
            std::cout << std::endl;
            for (auto const& state : final)
            {
                std::cout << state.id << " ";
            }

        }

        std::cout << std::endl;

        return std::includes(final.begin(), final.end(),
            nfa.accepted.begin(), nfa.accepted.end());
    }

    namespace util
    {
        std::set<State> epsilon_closure(NFA const& nfa, State const& state)
        {
            std::set<State> next{state};
            std::ranges::for_each(nfa.edges,
                [&next, &state](NFA::Branch const& branch) {
                    auto state_found = false;
                    std::ranges::for_each(branch,
                        [&next, &state, &state_found](Edge<State> const& edge) {
                            if (state_found == true and edge.symbol != util::Epsilon)
                            {
                                next.insert(edge.nodes.first);
                                //next.insert(edge.nodes.second);
                                state_found = false;
                            }
                            if (state_found == true and edge.nodes.second.type == State::Type::accept)
                            {
                                next.insert(edge.nodes.second);
                                state_found = false;
                            }
                            if ((edge.nodes.first == state or edge.nodes.second == state)
                                and edge.symbol == util::Epsilon)
                            {
                                state_found = true;
                            }
                        });
                });
            return next;
        }
        std::set<State> epsilon_closure(NFA const& nfa, std::set<State> states)
        {
            std::set<State> next{};
            std::ranges::for_each(states,
                [&nfa, &next](State const& state) {
                    next.merge(epsilon_closure(nfa, state));
                });
            return next.empty() ? states : next;
        }

        std::set<State> transition(NFA const& nfa, std::set<State> const& states, NFA::Input symbol)
        {
            std::set<State> next{};
            std::ranges::for_each(nfa.edges,
                [&next, &states, &symbol](NFA::Branch const& branch) {
                    auto state_found = false;
                    std::ranges::for_each(branch,
                        [&next, &states, &symbol, &state_found](Edge<State> const& edge) {
                            if (state_found == true)
                            {
                                //next.insert(edge.nodes.first);
                                next.insert(edge.nodes.second);
                                state_found = false;
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

int main()
{
    // auto nfa = ttre::util::construct_NFA_from_regular_expression("a|b");
    // auto step = ttre::util::epsilon_closure(nfa, nfa.start);
    // auto states = ttre::util::transition(nfa, step, 'a');
    // ttre::util::print_NFA(nfa, "a|b");
    ttre::print<"aaa">();
    std::cout << std::boolalpha << ttre::match<"aaa">("aa");
}
