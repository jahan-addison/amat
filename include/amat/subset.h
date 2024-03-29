#pragma once

#include <amat/nfa.h>
#include <ranges>

namespace amat {
namespace util {

using States = std::set<Edge::Node>;

// forward declarations
States
epsilon_closure(NFA const& nfa, Edge::Node const& state);
States
epsilon_closure(NFA const& nfa, States& states);
States
transition(NFA const& nfa, States const& states, NFA::Input symbol);

/**
 * Simulate an NFA by on-the-fly subset construction on `amat::NFA'.
 */

struct Simulator
{
    Simulator() = delete;
    Simulator(NFA nfa_)
      : nfa(nfa_)
    {
        old_states = nfa_.states;
    }

  public:
    void add_state(Edge::Node state)
    {
        this->new_states.emplace(state);
        this->on.emplace_back(state);
        auto epsilon_states = epsilon_closure(this->nfa, state);

        for (auto const& t : epsilon_states) {
            if (std::ranges::find(this->on, t) == this->on.end()) {
                this->add_state(t);
            }
        }
    }

    void get_next_closed_transition(NFA::Input c)
    {
        for (auto const& s : old_states) {
            for (auto const& t : transition(this->nfa, { s }, c)) {
                if (std::ranges::find(this->on, t) == this->on.end()) {
                    this->add_state(t);
                }
            }
        }
        old_states.clear();
        for (auto const& s : new_states) {
            this->old_states.emplace(s);
            if (auto location = std::ranges::find(this->on, s);
                location != this->on.end()) {
                this->on.erase(location);
            }
        }
        new_states.clear();
    }

  public:
    NFA nfa;
    States old_states{};
    States new_states{};
    std::vector<Edge::Node> on{};
};

States
epsilon_closure(NFA const& nfa, Edge::Node const& state)
{
    States next{};
    std::ranges::for_each(
      nfa.edges, [&next, &state](NFA::Branch const& branch) {
          auto state_found = false;
          for (auto const& edge : branch) {
              if (state_found == true and edge.symbol != util::Epsilon)
                  state_found = false;

              if (state_found == true and edge.symbol == util::Epsilon) {
                  if (edge.nodes.first.get()->id < edge.nodes.second.get()->id)
                      next.emplace(edge.nodes.first);
                  if (edge.nodes.second != state)
                      next.emplace(edge.nodes.second);
              }
              if (edge.nodes.first == state or edge.nodes.second == state) {
                  if (edge.nodes.first == state and
                      edge.symbol == util::Epsilon)
                      next.emplace(edge.nodes.second);
                  state_found = true;
              }
          }
      });
    if (next.empty())
        next.emplace(state);
    return next;
}

States
epsilon_closure(NFA const& nfa, States& states)
{
    States next{};
    std::ranges::for_each(states, [&nfa, &next](Edge::Node const& state) {
        next.merge(epsilon_closure(nfa, state));
    });
    return next;
}

States
transition(NFA const& nfa, States const& states, NFA::Input symbol)
{
    States next{};
    for (auto const& branch : nfa.edges) {
        auto state_found = false;
        for (auto const& edge : branch) {
            if (state_found == true) {
                if (edge.nodes.second->type != State::Type::accept)
                    next.emplace(edge.nodes.first);
                next.emplace(edge.nodes.second);
                state_found = false;
            }
            if ((states.contains(edge.nodes.first) or
                 states.contains(edge.nodes.second)) and
                edge.symbol == symbol) {
                state_found = true;
                if (edge.nodes.second->type == State::Type::accept)
                    next.emplace(edge.nodes.second);
            }
        }
    }
    return next;
}

} // namespace util

} // namespace amat