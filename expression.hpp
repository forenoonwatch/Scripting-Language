#pragma once

#include <vector>
#include <stack>
#include <string>
#include <memory>

#include "interpreter.hpp"
#include "variable.hpp"
#include "statement.hpp"
#include "token.hpp"

class Expression {
	public:
		Expression(Interpreter& interp, Statement* expr, std::shared_ptr<Variable> var);

		bool canEval() const;
		void evalNext();

		virtual ~Expression() = default;
	private:
		Interpreter& interpreter;

		std::stack<Variable> values;
		std::stack<std::string> operators;

		std::shared_ptr<Variable> writeVar;

		std::vector<Token>::iterator it;
		std::vector<Token>::iterator end;
};
