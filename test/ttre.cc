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

Edge::Node assert_and_get_state_exists_by_id(std::set<Edge::Node> const& items, unsigned short id)
{
    auto search = std::ranges::find_if(items.begin(), items.end(),
        [&id](Edge::Node const& search) -> bool {
            return search->id == id;
        });

    REQUIRE(search != items.end());
    return *search;
}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : kleene star case 1")
{
    auto fixture_1 = NFA_Fixture("a*bb");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 1);

    assert_and_get_state_exists_by_id(test, 2);

    test = util::epsilon_closure(fixture_1.nfa, *test.begin());

    CHECK(test.size() == 1);

}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : kleene star case 2")
{
    auto fixture_1 = NFA_Fixture("a*");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 3);

    assert_and_get_state_exists_by_id(test, 3);
    assert_and_get_state_exists_by_id(test, 1);
    assert_and_get_state_exists_by_id(test, 1);
}


TEST_CASE("ttre::util::epsilon_closure : overload 1 : character case")
{
    auto fixture_1 = NFA_Fixture("a");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 1);
}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : union case 1")
{
    auto fixture_1 = NFA_Fixture("a|b");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 2);

    assert_and_get_state_exists_by_id(test, 5);
    assert_and_get_state_exists_by_id(test, 2);
}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : union case 2")
{
    auto fixture_1 = NFA_Fixture("a*|b");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 3);

    assert_and_get_state_exists_by_id(test, 4);
    assert_and_get_state_exists_by_id(test, 7);
    assert_and_get_state_exists_by_id(test, 5);
}

TEST_CASE("ttre::util::epsilon_closure : overload 1 : concatenation case")
{
    auto fixture_1 = NFA_Fixture("aaa");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 1);
}


TEST_CASE("ttre::util::epsilon_closure : overload 2 : case 1")
{
    auto fixture_1 = NFA_Fixture("a*bb");

    std::set<Edge::Node> states = {
        assert_and_get_state_exists_by_id(fixture_1.nfa.states, 0),
        assert_and_get_state_exists_by_id(fixture_1.nfa.states, 1),
    };

    auto test = util::epsilon_closure(fixture_1.nfa, states);

    CHECK(test.size() == 2);

    assert_and_get_state_exists_by_id(test, 0);

}

TEST_CASE("ttre::util::epsilon_closure : overload 2 : case 2")
{
    auto fixture_1 = NFA_Fixture("a*|bb");

    // util::print_NFA(fixture_1.nfa, "a*|bb");
    // std::cout << std::boolalpha << "test: " << (assert_and_get_state_exists_by_id(fixture_1.nfa.states, 0) == fixture_1.nfa.start);

    std::set<Edge::Node> states = {
        fixture_1.nfa.start,
        assert_and_get_state_exists_by_id(fixture_1.nfa.states, 5),
    };

    auto test = util::epsilon_closure(fixture_1.nfa, states);

    assert_and_get_state_exists_by_id(fixture_1.nfa.states, 12);
    assert_and_get_state_exists_by_id(fixture_1.nfa.states, 4);
    assert_and_get_state_exists_by_id(fixture_1.nfa.states, 7);
    assert_and_get_state_exists_by_id(fixture_1.nfa.states, 5);
}


TEST_CASE("ttre::util::transition : kleene star case 1")
{
    auto fixture_1 = NFA_Fixture("a*bb");

    auto states = util::transition(fixture_1.nfa, {fixture_1.nfa.start}, 'a');

    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 1);
    assert_and_get_state_exists_by_id(states, 0);

    states = util::transition(fixture_1.nfa, fixture_1.nfa.states, 'b');

    CHECK(states.size() == 2);

    assert_and_get_state_exists_by_id(states, 5);
    assert_and_get_state_exists_by_id(states, 4);

}
