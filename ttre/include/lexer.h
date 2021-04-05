#ifndef TTRE_LEXER_H
#include <ttre/include/enum.h>

namespace TTre
{
    BETTER_ENUM(Tokens, unsigned short,
        T_EMPTY = 1,
        T_KLEENE_STAR,
        T_UNION,
        T_CHAR)
} // namespace TTre

#endif // TTRE_LEXER_H