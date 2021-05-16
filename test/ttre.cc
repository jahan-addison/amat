#include <test/catch.h>
#include <ttre/ttre.h>

#include <algorithm>
#include <set>
#include <iostream>

using namespace ttre;

struct NFA_Fixture
{
    NFA nfa;
    NFA_Fixture(NFA_Fixture const&) = delete;
    explicit NFA_Fixture(std::string_view str) : nfa(util::construct_NFA_from_regular_expression(str))
    {};
};

void assert_state_exists_by_id(std::set<Edge::Node> const& items, unsigned short id)
{
    auto search = std::ranges::find_if(items.begin(), items.end(),
        [&id](Edge::Node const& search) -> bool {
            return search->id == id;
        });

    REQUIRE(search != items.end());
}

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

    CHECK(test.size() == 1);

    assert_state_exists_by_id(test, 2);

    test = util::epsilon_closure(fixture_1.nfa, *test.begin());

    CHECK(test.size() == 0);

}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : kleene star case 2")
{
    auto fixture_1 = NFA_Fixture("a*");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 3);

    assert_state_exists_by_id(test, 3);
    assert_state_exists_by_id(test, 1);
    assert_state_exists_by_id(test, 1);
}


TEST_CASE("ttre::util::epsilon_closure : overload 1 : character case")
{
    auto fixture_1 = NFA_Fixture("a");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 0);
}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : union case 1")
{
    auto fixture_1 = NFA_Fixture("a|b");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 2);

    assert_state_exists_by_id(test, 5);
    assert_state_exists_by_id(test, 2);
}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : union case 2")
{
    auto fixture_1 = NFA_Fixture("a*|b");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 3);

    assert_state_exists_by_id(test, 4);
    assert_state_exists_by_id(test, 7);
    assert_state_exists_by_id(test, 5);
}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : concatenation case")
{
    auto fixture_1 = NFA_Fixture("aaa");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 0);
}

