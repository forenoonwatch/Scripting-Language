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

		virtual bool canGetStatement() const;
		Statement* peekStatement();
		Statement* getStatement();

		virtual bool isFunction() const;

		void addVariable(const std::string& name, std::shared_ptr<Variable> var);

		std::shared_ptr<Variable> getVariable(const std::string& name);

		virtual ~ScopeFrame() = default;
	protected:
		void writeVarsToVector(std::vector<std::shared_ptr<Variable>>&) const;
	private:
		std::map<std::string, std::shared_ptr<Variable>> variableMap;

		std::vector<Statement*>::iterator currStatement;
		std::vector<Statement*>::iterator end;
};

class FunctionFrame: public ScopeFrame {
	public:
		FunctionFrame(Interpreter& interpreter, Statement* scope, Statement* call);
		FunctionFrame(Interpreter& interpreter,
			Statement* call, std::shared_ptr<Variable> externalCall);
		
		virtual bool canGetStatement() const override;

		virtual bool isFunction() const override;
		bool isExternalFunction() const;

		bool canEvalArg() const;
		void evalNextArg();

		void setBaseExpression(int);
		int getBaseExpression() const;

		void doExternalCall();

		Variable& getReturnValue();

		void setReturning();
		bool getIsReturning() const;
	private:
		bool isReturning;
		int baseExpression;

		Variable returnValue;

		Interpreter& interpreter;
		
		std::shared_ptr<Variable> externalCall;

		std::vector<Statement*>::iterator arg;
		std::vector<Statement*>::iterator argEnd;

		Statement* params;
		
		std::vector<Token>::iterator paramName;
		std::vector<Token>::iterator paramEnd;
};

class LoopFrame: public ScopeFrame {
	public:
		LoopFrame(Statement* body, Statement* condition);
	private:
		Statement* body;
		Statement* condition;
};
