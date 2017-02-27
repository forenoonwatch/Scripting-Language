#include "token.hpp"

Token::Token(const std::string& content, Token::TokenType tokenType)
: content(content), tokenType(tokenType) {}

Token::Token(Statement* link)
: tokenType(Token::TokenType::LINK), link(link) {}
