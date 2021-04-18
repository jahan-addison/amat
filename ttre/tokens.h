#ifndef TTRE_TOKENS_H
#include <ttre/enum.h>

namespace TTre
{
    BETTER_ENUM(Token, unsigned short,
        T_KLEENE_STAR = 1,
        T_UNION,
        T_CHAR,
        T_UNKNOWN,
        T_END)

        constexpr const char* as_string(Token token)
    {
        switch (token)
        {
        case Token::T_KLEENE_STAR: return "T_KLEENE_STAR";
        case Token::T_UNION: return "T_UNION";
        case Token::T_CHAR: return "T_CHAR";
        case Token::T_UNKNOWN: return "T_UNKNOWN";
        case Token::T_END: return "T_END";
        }

        return "T_UNKNOWN";
    }

    constexpr auto token_as_string = better_enums::make_map(as_string);

} // namespace TTre
#endif