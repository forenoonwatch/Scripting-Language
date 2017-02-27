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
		bool canConsumeStatement() const;

		virtual ~Parser() = default;
	private:
		Interpreter& interpreter;
		Statement* root;
		Statement* currRoot;

		void consumeVarDeclaration();
		void consumeIfStatement();
		void consumeFunDeclaration();

		void consumeExpression();
		void consumeFunctionCall();

		bool acceptToken(const Token&, unsigned);
		bool acceptToken(const Token&, Token::TokenType, const std::string&);

		friend class Interpreter;
};
