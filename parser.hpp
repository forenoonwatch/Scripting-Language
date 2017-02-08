#pragma once

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
};
