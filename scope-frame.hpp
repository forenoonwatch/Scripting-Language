#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "variable.hpp"
#include "statement.hpp"
#include "interpreter.hpp"

class ScopeFrame {
	public:
		ScopeFrame(Statement*);

		bool canGetStatement() const;
		Statement* peekStatement();
		Statement* getStatement();

		virtual bool isFunction() const;

		void addVariable(const std::string& name, std::shared_ptr<Variable> var);

		std::shared_ptr<Variable> getVariable(const std::string& name);

		virtual ~ScopeFrame() = default;
	private:
		std::map<std::string, std::shared_ptr<Variable>> variableMap;

		std::vector<Statement*>::iterator currStatement;
		std::vector<Statement*>::iterator end;
};

class FunctionFrame: public ScopeFrame {
	public:
		FunctionFrame(Interpreter& interpreter, Statement* scope, Statement* call);
		
		virtual bool isFunction() const override;

		bool canEvalArg() const;
		void evalNextArg();

		Variable& getReturnValue();

	private:
		Variable returnValue;

		Interpreter& interpreter;
		
		std::vector<Statement*>::iterator arg;
		std::vector<Statement*>::iterator argEnd;

		Statement* params;
		
		std::vector<Token>::iterator paramName;
		std::vector<Token>::iterator paramEnd;
};
