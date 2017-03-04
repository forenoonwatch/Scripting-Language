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
		void consumeVarAssignment();
		
		void consumeIfStatement();

		void consumeWhileLoop();
		void consumeForLoop();

		void consumeFunDeclaration();
		void consumeAnonymousFunDecl();

		void consumeOtherIdentifier();

		void consumeExpression();
		void consumeFunctionCall();
		void consumeParamDeclaration();
		void consumeScopeBlock();

		bool acceptToken(const Token&, unsigned);
		bool acceptToken(const Token&, Token::TokenType, const std::string&);

		friend class Interpreter;
};
