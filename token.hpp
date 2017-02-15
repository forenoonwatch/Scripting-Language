#pragma once

#include <string>

class Token {
	public:
		enum TokenType {
			IDENTIFIER,
			NUMERIC,
			STRING,
			OPERATOR,
			OTHER
		};

		Token(const std::string& content, TokenType tokenType);

		std::string getContent() const;
		TokenType getTokenType() const;

		virtual ~Token() = default;
	private:
		std::string content;
		TokenType tokenType;
};

#include "token.inl"
