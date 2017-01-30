#include "token-stream.hpp"

void TokenStream::addToken(const std::string& content,
		Token::TokenType tokenType) {
	tokens.emplace_back(content, tokenType);
}

void TokenStream::addToken(Token token) {
	tokens.push_back(token);
}
