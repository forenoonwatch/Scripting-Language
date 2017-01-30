#pragma once

#include <vector>
#include <string>
#include "token.hpp"

class TokenStream {
	public:
		TokenStream() = default;

		void addToken(const std::string& content, Token::TokenType tokenType);
		void addToken(Token token);

		virtual ~TokenStream() = default;
	private:
		std::vector<Token> tokens;
};
