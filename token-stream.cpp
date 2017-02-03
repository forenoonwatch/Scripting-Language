#include "token-stream.hpp"

TokenStream::TokenStream()
: streamI(0) {}

void TokenStream::addToken(const std::string& content,
		Token::TokenType tokenType) {
	tokens.emplace_back(content, tokenType);
}

void TokenStream::addToken(Token token) {
	tokens.push_back(token);
}

Token TokenStream::peek() const {
	return tokens[streamI];
}

Token TokenStream::get() {
	return tokens[streamI++];
}

bool TokenStream::canGet() const {
	return streamI != tokens.size();
}
