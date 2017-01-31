#pragma once

#include <iostream>
#include "token.hpp"
#include "token-stream.hpp"

class Lexer {
	public:
		Lexer(std::istream& textStream, TokenStream& tokenStream);

		/**
		 * @brief consumes one token from the text stream and adds it to the token stream
		 */ 
		void consumeNextToken();

		bool canConsumeToken() const;

		virtual ~Lexer() = default;
	private:
		std::istream& textStream;
		TokenStream& tokenStream;

		void consumeWhitespace();
		void consumeIdentifier();
		void consumeNumericLiteral();
		void consumeStringLiteral();
		void consumeOperator();

		static bool isWhitespace(char);
		static bool isIdentifierStart(char);
		static bool isIdentifier(char);
		static bool isNumericLiteral(char);
		static bool isStringLiteral(char); // TODO: add operator check
};