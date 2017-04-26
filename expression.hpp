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
		Expression(Interpreter& interp, Statement* expr, std::shared_ptr<Variable> var,
			int startOffset = 0);

		bool canEval() const;
		void evalNext();
		void finishEval();

		void addValue(const Variable& value);

		bool isExpectingValue() const;

		virtual ~Expression() = default;
	private:
		Interpreter& interpreter;

		bool expectValue;

		int startOffset;

		std::stack<Variable> values;
		std::stack<std::string> operators;
		std::stack<std::string> unaryOps;

		Token lastToken;

		std::shared_ptr<Variable> writeVar;

		std::vector<Token>::iterator it;
		std::vector<Token>::iterator end;
};
