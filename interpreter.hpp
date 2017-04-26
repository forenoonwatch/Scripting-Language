#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "token-stream.hpp"
#include "operator-registry.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "error-log.hpp"
#include "variable.hpp"
#include "scope-frame.hpp"
#include "expression.hpp"

class Lexer;
class Parser;
class Expression;
class ScopeFrame;
class FunctionFrame;

class Interpreter {
	public:
		Interpreter(std::istream& textStream);

		void parseText();
		void interpretNextStatement();

		bool canInterpretStatement() const;

		void createVariable(const std::string& name, const std::string& value);
		void createVariable(const std::string& name, int value);
		void createVariable(const std::string& name, double value);
		void createVariable(const std::string& name, bool value);

		void setVariable(const std::string& name, const std::string& value);
		void setVariable(const std::string& name, int value);
		void setVariable(const std::string& name, double value);
		void setVariable(const std::string& name, bool value);

		void evalExpression(Statement*, std::shared_ptr<Variable>, int startOffset = 0);
	
		std::shared_ptr<Variable> getVariable(const std::string& varName);

		virtual ~Interpreter() = default;
	private:
		TokenStream tokenStream;
		OperatorRegistry operatorRegistry;
		ErrorLog errorLog;

		bool canContinue;
		bool evaluateExpression;
		bool haltFunction;

		std::shared_ptr<Variable> returnProxy;

		std::unique_ptr<Lexer> lexer;
		std::unique_ptr<Parser> parser;

		std::vector<std::shared_ptr<ScopeFrame>> scopeStack;
		std::vector<std::shared_ptr<Expression>> expressionStack;

		std::shared_ptr<Variable> resolveVariable(const std::string& name,
			int offsetCount = 0);

		void lexAllTokens();
		void parseAllStatements(); // TODO: possibly interpret while parsing

		void interpretVarDecl(Statement*);
		void interpretVarAssignment(Statement*);
		void interpretIfStatement(Statement*);

		void interpretFuncDecl(Statement*);
		void interpretReturn(Statement*);

		void evalCallArgs(std::shared_ptr<FunctionFrame> func, Statement* body, Statement* call);

		friend class Lexer;
		friend class Parser;
		friend class Expression;
};
