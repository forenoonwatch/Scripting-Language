#pragma once

#include <string>
#include <memory>
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
		std::shared_ptr<Statement> root;
		std::shared_ptr<Statement> currRoot;

		void consumeVarDeclaration();
		void consumeIfStatement();
		void consumeFunDeclaration();

		bool acceptToken(const Token&, Token::TokenType);
		bool acceptToken(const Token&, Token::TokenType, const std::string&);

		friend class Interpreter;
};
