#pragma once

#include <iostream>
#include <string>
#include "token.hpp"
#include "token-stream.hpp"
#include "interpreter.hpp"
#include "error-log.hpp"

class Interpreter;

class Lexer {
	public:
		Lexer(std::istream& textStream, Interpreter& interpreter);

		/**
		 * @brief consumes one token from the text stream and adds it to the token stream
		 */ 
		void consumeNextToken();

		bool canConsumeToken() const;
		bool hasErrored() const;

		virtual ~Lexer() = default;
	private:
		std::istream& textStream;
		Interpreter& interpreter;
		bool canContinue;

		void consumeWhitespace();
		void consumeIdentifier();
		void consumeNumericLiteral();
		void consumeStringLiteral();
		void consumeOperator();

		void errorNextChar(const std::string&);

		static bool isWhitespace(char);
		static bool isIdentifierStart(char);
		static bool isIdentifier(char);
		static bool isNumericLiteral(char);
		static bool isStringLiteral(char);
		static bool isPossibleOperator(char); 
};
