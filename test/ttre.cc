#include <test/catch.h>
#include <ttre/ttre.h>

#include <iostream>

using namespace ttre;

struct NFA_Fixture
{
    NFA nfa;
    NFA_Fixture(NFA_Fixture const&) = delete;
    explicit NFA_Fixture(std::string_view str) : nfa(util::construct_NFA_from_regular_expression(str))
    {};
};

void log_all_NFA_states(NFA const& nfa)
{
    std::cout << "States: " << std::endl;
    for (auto const& state : nfa.states)
    {
        std::cout << "State: " << state.get()->id << " " << state.get()->type << std::endl;
    }
}

void log_all_NFA_states(std::set<Edge::Node> const& states)
{
    std::cout << "States: " << std::endl;
    for (auto const& state : states)
    {
        std::cout << "State: " << state.get()->id << " " << state.get()->type << std::endl;
    }
}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : kleene star case 1")
{
    auto fixture_1 = NFA_Fixture("a*bb");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    // util::print_NFA(fixture_1.nfa, "a*bb");

    // log_all_NFA_states(fixture_1.nfa);
    // log_all_NFA_states(test);

    auto iterator_test = test.begin();
    auto iterator_expect = fixture_1.nfa.states.begin();

    CHECK(test.size() == 1);

    std::advance(iterator_expect, 4);

    CHECK(iterator_test->get() == iterator_expect->get());

}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : kleene star case 2")
{
    auto fixture_1 = NFA_Fixture("a*");
    // auto fixture_3 = NFA_Fixture("a");
    // auto fixture_4 = NFA_Fixture("a|b");
    // auto fixture_5 = NFA_Fixture("aaa");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    // util::print_NFA(fixture_1.nfa, "a*");
    // log_all_NFA_states(fixture_1.nfa);
    // log_all_NFA_states(test);

    // auto iterator_test = test.begin();
    // auto iterator_expect = fixture_1.nfa.states.begin();

    // CHECK(iterator_test->get() == iterator_expect->get());

    // std::advance(iterator_test, 1);
    // std::advance(iterator_expect, 1);

    // CHECK(iterator_test->get() == iterator_expect->get());

}
