#pragma once

#include <vector>
#include <string>
#include "token.hpp"

class TokenStream {
	public:
		TokenStream();

		void addToken(const std::string& content, Token::TokenType tokenType);
		void addToken(Token token);

		Token peek() const;
		Token peekNext() const;
		Token get();

		bool canGet() const;
		bool canGetNext() const;

		virtual ~TokenStream() = default;
	private:
		std::vector<Token> tokens;
		unsigned streamI;
};
