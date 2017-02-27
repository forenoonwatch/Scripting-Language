#pragma once

#include <string>
#include "statement.hpp"

class Statement;

class Token {
	public:
		enum TokenType : unsigned {
			IDENTIFIER = 0x001,
			NUMERIC = 0x002,
			STRING = 0x004,
			OPERATOR = 0x008,
			LINK = 0x010,
			OTHER = 0x020,
		};

		Token(const std::string& content, TokenType tokenType);
		Token(Statement* link);

		std::string getContent() const;
		TokenType getTokenType() const;

		Statement* getLink();

		virtual ~Token() = default;
	private:
		std::string content;
		TokenType tokenType;
		Statement* link;
};

#include "token.inl"
