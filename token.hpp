#pragma once

#include <string>

class Token {
	public:
		enum TokenType : unsigned {
			IDENTIFIER = 0x001,
			NUMERIC = 0x002,
			STRING = 0x004,
			OPERATOR = 0x008,
			OTHER = 0x010
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
