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
#include "default-lib.hpp"

class Lexer;
class Parser;

class Expression;
class ScopeFrame;
class FunctionFrame;

class DefaultLibrary;

class Interpreter {
	public:
		Interpreter(std::istream& textStream, bool useDefaultLib = true);

		void parseText();

		bool isRunning() const;
		bool hasScriptEvents() const;

		void processScript();

		bool isYielded() const;
		void setYielded(bool);

		void addExternalFunc(const std::string& name, ExternalFunction func);

		void createVariable(const std::string& name, const std::string& value);
		void createVariable(const std::string& name, int value);
		void createVariable(const std::string& name, double value);
		void createVariable(const std::string& name, bool value);

		void setVariable(const std::string& name, const std::string& value);
		void setVariable(const std::string& name, int value);
		void setVariable(const std::string& name, double value);
		void setVariable(const std::string& name, bool value);
	
		std::shared_ptr<Variable> getVariable(const std::string& varName);

		virtual ~Interpreter() = default;
	private:
		TokenStream tokenStream;
		OperatorRegistry operatorRegistry;
		ErrorLog errorLog;

		bool canContinue;
		bool scriptYielded;
		bool evaluateExpression;

		std::shared_ptr<Variable> returnProxy;

		std::unique_ptr<Lexer> lexer;
		std::unique_ptr<Parser> parser;
		std::unique_ptr<DefaultLibrary> defaultLib;

		std::vector<std::shared_ptr<ScopeFrame>> scopeStack;
		std::vector<std::shared_ptr<Expression>> expressionStack;
		std::vector<std::shared_ptr<FunctionFrame>> functionStack;

		void interpretNextStatement();
		bool canInterpretStatement() const;

		std::shared_ptr<Variable> resolveVariable(const std::string& name,
			int offsetCount = 0);

		void evalExpression(Statement*, std::shared_ptr<Variable>, int startOffset = 0);

		void addFunctionCall(Statement* body, Statement* call);
		void addFunctionCall(Statement* scope, std::shared_ptr<Variable> externalCall);

		void lexAllTokens();
		void parseAllStatements(); // TODO: possibly interpret while parsing

		void interpretVarDecl(Statement*);
		void interpretVarAssignment(Statement*);
		void interpretIfStatement(Statement*);

		void interpretFuncDecl(Statement*);
		void interpretFuncCall(Statement*);
		void interpretReturn(Statement*);

		void cleanAfterExpression();

		friend class Lexer;
		friend class Parser;
		friend class Expression;
		friend class FunctionFrame;
		friend class DefaultLibrary;
};
