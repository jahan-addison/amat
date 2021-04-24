#pragma once

namespace ttre
{
    enum class Token : unsigned short
    {
        T_OPEN_PAREN = 1,
        T_CLOSE_PAREN,
        T_KLEENE_STAR,
        T_UNION,
        T_CHAR,
        T_UNKNOWN,
        T_END
    };

    constexpr const char* token_as_string(Token token)
    {
        switch (token)
        {
        case Token::T_OPEN_PAREN: return "T_OPEN_PAREN";
        case Token::T_CLOSE_PAREN: return "T_CLOSE_PAREN";
        case Token::T_KLEENE_STAR: return "T_KLEENE_STAR";
        case Token::T_UNION: return "T_UNION";
        case Token::T_CHAR: return "T_CHAR";
        case Token::T_UNKNOWN: return "T_UNKNOWN";
        case Token::T_END: return "T_END";
        }

        return "T_UNKNOWN";
    }

} // namespace ttre