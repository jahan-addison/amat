#ifndef TTRE_LEXER_H
#include <ttre/tokens.h>
#include <string_view>
#include <optional>

namespace TTre
{
    class Lexer
    {
    public:
        Lexer(Lexer const&) = default;
        explicit Lexer(std::string_view str) : source_(str),
            pointer_(0),
            current_(Token::T_UNKNOWN),
            last_(Token::T_UNKNOWN)
        {}

    public:
        Token get_next_token();
        inline std::optional<unsigned char> scanner() const
        {
            return scanner_;
        }
        inline unsigned int pointer()
        {
            return pointer_;
        }

    public:
        Lexer operator++(int);
        inline Token operator*() const
        {
            return current_;
        }

    private:
        std::string_view source_;
        unsigned int pointer_;
        std::optional<unsigned char> scanner_;
        Token current_;
        Token last_;
    };

} // namespace TTre

#endif // TTRE_LEXER_H