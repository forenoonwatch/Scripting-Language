#pragma once

#include <string>

class Token {
	public:
		enum TokenType {
			KEYWORD,
			NUMBER,
			STRING,
			OPERATOR
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
