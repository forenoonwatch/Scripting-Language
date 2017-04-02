#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "variable.hpp"
#include "statement.hpp"

class ScopeFrame {
	public:
		ScopeFrame(Statement*);

		bool canGetStatement() const;
		Statement* peekStatement();
		Statement* getStatement();

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
		FunctionFrame(Statement*);
		
		Variable& getReturnValue();

	private:
		Variable returnValue;
};
