#ifndef TTRE_TOKENS_H
#include <ttre/include/enum.h>

namespace TTre
{
    BETTER_ENUM(Token, unsigned short,
        T_KLEENE_STAR = 1,
        T_UNION,
        T_CHAR,
        T_UNKNOWN,
        T_END)
} // namespace TTre
#endif