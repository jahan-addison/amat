#include <ttre/parser.h>
#include <stdexcept>
#include <iostream>

namespace TTre
{
    void Parser::parse_kleene_star_()
    {
        this->is_concat_ = false;
        this->operators_.push_back('*');
    }

    void Parser::parse_union_()
    {
        this->is_concat_ = false;
        if (this->operators_.length())
        {
            if (get_order_from_operator_char(this->operators_.back()) >= get_order_from_operator_char('|')
                and this->operators_.back() != '(')
            {
                unsigned char last = static_cast<unsigned char>(this->operators_.back());
                this->operators_.pop_back();
                this->output_.push_back(last);
            }
        }
        this->operators_.push_back('|');
    }

    void Parser::parse_char_()
    {
        if (this->is_concat_)
        {
            if (this->operators_.length())
            {
                if (get_order_from_operator_char(this->operators_.back()) >= get_order_from_operator_char('.')
                    and this->operators_.back() != '(')
                {
                    unsigned char last = static_cast<unsigned char>(this->operators_.back());
                    this->operators_.pop_back();
                    this->output_.push_back(last);
                }
            }
            this->operators_.push_back('.');
        }
        else if (this->lexer_.pointer() == this->source_.length()
            and this->operators_.back() == '*')
        {
            this->operators_.push_back('.');
        }
        this->is_concat_ = true;
        this->output_.push_back(this->lexer_.scanner().value());
    }

    void Parser::parse_left_paren_()
    {
        this->operators_.push_back('(');
    }

    void Parser::parse_right_paren_()
    {
        while (this->operators_.length() and this->operators_.back() != '(')
        {
            auto last = this->operators_.back();
            this->operators_.pop_back();
            this->output_.push_back(last);
        }
        if (!this->operators_.length())
        {
            throw std::runtime_error("parse error: unclosed parenthesis pair");
        }
        if (this->operators_.back() == '(')
        {
            this->operators_.pop_back();

        }
    }

    std::string_view Parser::parse()
    {
        while (*this->lexer_++ != TTre::Token::T_END)
        {
            auto token = *this->lexer_;
            switch (token)
            {
            case Token::T_CHAR:
                this->parse_char_();
                break;
            case Token::T_KLEENE_STAR:
                this->parse_kleene_star_();
                break;
            case Token::T_UNION:
                this->parse_union_();
                break;
            case Token::T_OPEN_PAREN:
                this->parse_left_paren_();
                break;
            case Token::T_CLOSE_PAREN:
                this->parse_right_paren_();
                break;

            default:
                throw std::runtime_error("parse error: invalid operator");
            }
        }
        if (this->operators_.front() == '(' or this->operators_.back() == '(')
        {
            throw std::runtime_error("parse error: unclosed parenthesis pair");
        }

        while (this->operators_.length())
        {
            unsigned char last = static_cast<unsigned char>(this->operators_.back());
            this->operators_.pop_back();
            this->output_.push_back(last);
        }
        return this->output_;
    }
} // namespace TTre