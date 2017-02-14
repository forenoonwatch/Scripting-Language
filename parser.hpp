#pragma once

#include <string>
#include "interpreter.hpp"
#include "token.hpp"
#include "statement.hpp"

class Interpreter;

class Parser {
	public:
		Parser(Interpreter&);

		void consumeNextStatement();

		virtual ~Parser() = default;
	private:
		Interpreter& interpreter;
		Statement root;
		Statement& currRoot;

		void consumeVarDeclaration();
		void consumeIfStatement();
		void consumeFunDeclaration();

		static bool acceptToken(const Token&, Token::TokenType);
		static bool acceptToken(const Token&, Token::TokenType, const std::string&);
};
