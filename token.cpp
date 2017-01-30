#include "token.hpp"

Token::Token(const std::string& content, Token::TokenType tokenType)
: content(content), tokenType(tokenType) {}
