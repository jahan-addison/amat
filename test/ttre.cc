#include <test/catch.h>
#include <ttre/ttre.h>
#include <iostream>

class NFA_Fixture
{
public:
    ttre::NFA nfa;
    NFA_Fixture(NFA_Fixture const&) = delete;
    explicit NFA_Fixture(std::string_view str) : nfa(ttre::util::construct_NFA_from_regular_expression(str))
    {};
};


TEST_CASE("ttre::util::epsilon_closure : overload 1")
{
    using namespace ttre;

    auto fixture_1 = NFA_Fixture("a*b");
    // auto fixture_2 = NFA_Fixture("a*");
    // auto fixture_3 = NFA_Fixture("a");
    // auto fixture_4 = NFA_Fixture("a|b");
    // auto fixture_5 = NFA_Fixture("aaa");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    auto iterator_test = test.begin();
    auto iterator_expect = fixture_1.nfa.states.begin();

    std::advance(iterator_expect, 3);

    CHECK(iterator_test->get() == iterator_expect->get());

    std::advance(iterator_test, 1);
    std::advance(iterator_expect, 1);

    CHECK(iterator_test->get() == iterator_expect->get());

}