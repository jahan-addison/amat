#include <ttre/ttre.h>
#include <ranges>

namespace ttre
{
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
            if (next.empty())
                next.emplace(state);
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
            for (auto const& branch : nfa.edges)
            {
                auto state_found = false;
                for (auto const& edge : branch)
                {
                    if (state_found == true)
                    {
                        if (edge.nodes.second->type != State::Type::accept)
                            next.emplace(edge.nodes.first);
                        next.emplace(edge.nodes.second);
                        state_found = false;
                    }
                    if ((states.contains(edge.nodes.first) or states.contains(edge.nodes.second))
                        and edge.symbol == symbol)
                    {
                        state_found = true;
                        if (edge.nodes.second->type == State::Type::accept)
                            next.emplace(edge.nodes.second);
                    }
                }
            }
            return next;
        }

    } // namespace util

} // namespace ttre
