#include <amat.h>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <iostream>

using namespace amat;

struct NFA_Fixture
{
    NFA nfa;
    NFA_Fixture(NFA_Fixture const&) = delete;
    explicit NFA_Fixture(std::string_view str)
      : nfa(util::construct_NFA_from_regular_expression(str)){};
};

Edge::Node
assert_and_get_state_exists_by_id(std::set<Edge::Node> const& items,
                                  unsigned short id)
{
    auto search = std::ranges::find_if(
      items.begin(), items.end(), [&id](Edge::Node const& search) -> bool {
          return search->id == id;
      });

    if (search == items.end())
        WARN("id " << id << " not found");
    REQUIRE(search != items.end());
    return *search;
}

TEST_CASE("amat::util::epsilon_closure : overload 1 : kleene star case 1")
{
    auto fixture_1 = NFA_Fixture("a*bb");
    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);
    CHECK(test.size() == 1);
    assert_and_get_state_exists_by_id(test, 2);
    test = util::epsilon_closure(fixture_1.nfa, *test.begin());
    CHECK(test.size() == 2);
    test = util::epsilon_closure(
      fixture_1.nfa,
      assert_and_get_state_exists_by_id(fixture_1.nfa.states, 2));
    CHECK(test.size() == 2);
    assert_and_get_state_exists_by_id(test, 4);
    assert_and_get_state_exists_by_id(test, 0);
}

TEST_CASE("amat::util::epsilon_closure : overload 1 : kleene star case 2")
{
    auto fixture_1 = NFA_Fixture("a*");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 3);

    assert_and_get_state_exists_by_id(test, 3);
    assert_and_get_state_exists_by_id(test, 1);
    assert_and_get_state_exists_by_id(test, 1);
}

TEST_CASE("amat::util::epsilon_closure : overload 1 : character case")
{
    auto fixture_1 = NFA_Fixture("a");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 1);
}

TEST_CASE("amat::util::epsilon_closure : overload 1 : union case 1")
{
    auto fixture_1 = NFA_Fixture("a|b");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 2);

    assert_and_get_state_exists_by_id(test, 5);
    assert_and_get_state_exists_by_id(test, 2);
}

TEST_CASE("amat::util::epsilon_closure : overload 1 : union case 2")
{
    auto fixture_1 = NFA_Fixture("a*|b");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 3);

    assert_and_get_state_exists_by_id(test, 4);
    assert_and_get_state_exists_by_id(test, 7);
    assert_and_get_state_exists_by_id(test, 5);
}

TEST_CASE("amat::util::epsilon_closure : overload 1 : concatenation case")
{
    auto fixture_1 = NFA_Fixture("aaa");

    auto test = util::epsilon_closure(fixture_1.nfa, fixture_1.nfa.start);

    CHECK(test.size() == 1);
}

TEST_CASE("amat::util::epsilon_closure : overload 2 : case 1")
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

TEST_CASE("amat::util::epsilon_closure : overload 2 : case 2")
{
    auto fixture_1 = NFA_Fixture("a*|bb");

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

TEST_CASE("amat::util::transition : union case 1")
{
    auto fixture_1 = NFA_Fixture("(ab)|cde");
    auto states = util::transition(fixture_1.nfa, fixture_1.nfa.states, 'a');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 5);
    assert_and_get_state_exists_by_id(states, 4);
    states = util::transition(fixture_1.nfa, states, 'b');
    CHECK(states.size() == 1);
    assert_and_get_state_exists_by_id(states, 14);
    states = util::transition(fixture_1.nfa, fixture_1.nfa.states, 'c');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 10);
    assert_and_get_state_exists_by_id(states, 9);
    states = util::transition(fixture_1.nfa, states, 'd');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 11);
    assert_and_get_state_exists_by_id(states, 12);
    states = util::transition(fixture_1.nfa, states, 'e');
    CHECK(states.size() == 1);
    assert_and_get_state_exists_by_id(states, 14);
}

TEST_CASE("amat::util::transition : union case 2")
{
    auto fixture_1 = NFA_Fixture("ab|aaa");
    auto states = util::transition(fixture_1.nfa, fixture_1.nfa.states, 'a');
    states = util::transition(fixture_1.nfa, fixture_1.nfa.states, 'b');
    CHECK(states.size() == 1);
    assert_and_get_state_exists_by_id(states, 14);
    states = util::transition(fixture_1.nfa, fixture_1.nfa.states, 'a');
    states = util::transition(fixture_1.nfa, states, 'a');
    states = util::transition(fixture_1.nfa, states, 'a');
    CHECK(states.size() == 1);
    assert_and_get_state_exists_by_id(states, 14);
}

TEST_CASE("amat::util::transition : union case 3")
{
    // auto fixture_1 = NFA_Fixture("ab*");
    // auto states = util::transition(fixture_1.nfa, fixture_1.nfa.states, 'a');
    // states = util::transition(fixture_1.nfa, states, 'b');
}

TEST_CASE("amat::util::transition : kleene star case 1")
{
    auto fixture_1 = NFA_Fixture("(acab)*");
    auto states = util::transition(fixture_1.nfa, { fixture_1.nfa.start }, 'b');
    CHECK(states.size() == 0);
    states = util::transition(fixture_1.nfa, { fixture_1.nfa.start }, 'a');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 2);
    assert_and_get_state_exists_by_id(states, 3);
    auto test = util::transition(fixture_1.nfa, states, 'b');
    CHECK(test.size() == 0);
    states = util::transition(fixture_1.nfa, states, 'c');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 4);
    assert_and_get_state_exists_by_id(states, 5);
    states = util::transition(fixture_1.nfa, states, 'a');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 6);
    assert_and_get_state_exists_by_id(states, 7);
    states = util::transition(fixture_1.nfa, states, 'b');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 7);
    assert_and_get_state_exists_by_id(states, 0);
    // test the cyclic edge of the transition graph
    states = util::transition(fixture_1.nfa, states, 'a');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 2);
    assert_and_get_state_exists_by_id(states, 3);
}

TEST_CASE("amat::util::transition : concatenation case")
{
    auto fixture_1 = NFA_Fixture("aaaa*");
    auto states = util::transition(fixture_1.nfa, { fixture_1.nfa.start }, 'a');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 2);
    assert_and_get_state_exists_by_id(states, 3);
    states = util::transition(fixture_1.nfa, states, 'a');
    auto cyclic = assert_and_get_state_exists_by_id(fixture_1.nfa.states, 5);
    states = util::transition(fixture_1.nfa, { cyclic }, 'a');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 5);
    assert_and_get_state_exists_by_id(states, 7);
}

TEST_CASE("amat::util::transition : strong case")
{
    auto fixture_1 = NFA_Fixture("(ab)*|cd|abc");
    auto states =
      util::transition(fixture_1.nfa, { fixture_1.nfa.states }, 'a');

    CHECK(states.size() == 4);
    assert_and_get_state_exists_by_id(states, 24);
    assert_and_get_state_exists_by_id(states, 8);
    assert_and_get_state_exists_by_id(states, 6);
    assert_and_get_state_exists_by_id(states, 25);

    auto states_1 = util::transition(fixture_1.nfa, states, 'b');

    CHECK(states_1.size() == 4);
    // cyclic edges:
    assert_and_get_state_exists_by_id(states_1, 8);
    assert_and_get_state_exists_by_id(states_1, 9);
    // "c"
    assert_and_get_state_exists_by_id(states_1, 27);
    assert_and_get_state_exists_by_id(states_1, 26);

    states = util::transition(fixture_1.nfa, states_1, 'c');

    CHECK(states.size() == 2);

    assert_and_get_state_exists_by_id(states, 28);
    assert_and_get_state_exists_by_id(states, 29);

    states = util::transition(fixture_1.nfa, { fixture_1.nfa.states }, 'c');

    CHECK(states.size() == 4);

    assert_and_get_state_exists_by_id(states, 15);
    assert_and_get_state_exists_by_id(states, 16);
    assert_and_get_state_exists_by_id(states, 29);
    assert_and_get_state_exists_by_id(states, 28);

    states = util::transition(fixture_1.nfa, states, 'd');

    CHECK(states.size() == 2);

    assert_and_get_state_exists_by_id(states, 17);
    assert_and_get_state_exists_by_id(states, 29);
}

// TEST_CASE("amat::util::transition : kleene star case 2")
// {
//     auto fixture_1 = NFA_Fixture("(ac)*bb");
//     auto states = util::transition(fixture_1.nfa, {fixture_1.nfa.states},
//     'a'); util::print_NFA(fixture_1.nfa, "(ac)*bb");
//     util::print_states(states);

//     CHECK(states.size() == 2);
//     assert_and_get_state_exists_by_id(states, 2);
//     assert_and_get_state_exists_by_id(states, 3);
//     states = util::transition(fixture_1.nfa, states, 'c');

//     assert_and_get_state_exists_by_id(states, 0); // the cyclic edge
//     assert_and_get_state_exists_by_id(states, 3);
//     states = util::transition(fixture_1.nfa,
//     {assert_and_get_state_exists_by_id(fixture_1.nfa.states, 4)}, 'b');
//     CHECK(states.size() == 1);
//     assert_and_get_state_exists_by_id(states, 7);
//     states = util::transition(fixture_1.nfa, states, 'b');
//     CHECK(states.size() == 1);
//     assert_and_get_state_exists_by_id(states, 7);
// }

TEST_CASE("amat::util::transition : kleene star case 3")
{
    auto fixture_1 = NFA_Fixture("aac*bb");
    auto states =
      util::transition(fixture_1.nfa, { fixture_1.nfa.states }, 'a');
    CHECK(states.size() == 3);
    assert_and_get_state_exists_by_id(states, 2);
    assert_and_get_state_exists_by_id(states, 3);
    assert_and_get_state_exists_by_id(states, 6);

    states = util::transition(fixture_1.nfa, states, 'a');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 6);
    assert_and_get_state_exists_by_id(states, 3);
    states = util::transition(fixture_1.nfa, states, 'c');
    CHECK(states.size() == 2);
    assert_and_get_state_exists_by_id(states, 6);
    assert_and_get_state_exists_by_id(states, 3);
    states = util::transition(
      fixture_1.nfa,
      { assert_and_get_state_exists_by_id(fixture_1.nfa.states, 6) },
      'b');
    CHECK(states.size() == 1);
    assert_and_get_state_exists_by_id(states, 9);
    states = util::transition(fixture_1.nfa, states, 'b');
    CHECK(states.size() == 1);
    assert_and_get_state_exists_by_id(states, 9);
}

TEST_CASE("amat::match")
{
    // print<"abc|aaa">();
    // std::cout << std::boolalpha << " " << match<"abc|aaa">("abc");
    print<"abc|def">();
    CHECK(match<"abc|aaa">("abc") == true);
}
