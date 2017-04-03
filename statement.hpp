#pragma once

#include <vector>
#include <string>
#include "token.hpp"

class Token;

class Statement {
	public:
		enum StatementType {
			VAR_DECLARATION,
			VAR_ASSIGNMENT,
			EXPRESSION,
			FUNC_CALL,
			GROUPING,
			CONDITIONAL,
			FUNC_DECL,
			RETURN,
			WHILE_LOOP,
			FOR_LOOP,
			OTHER
		};
		
		Statement(StatementType type = OTHER, Statement* parent = nullptr);

		void addChild(Statement* child);
		void addToken(Token token);

		Statement* getParent();
		std::vector<Statement*>& getChildren();
		std::vector<Token>& getTokens();

		StatementType getType() const;

		virtual ~Statement() = default;

		static std::string typeAsString(StatementType type);
	private:
		StatementType type;

		Statement* parent;
		std::vector<Statement*> children;
		std::vector<Token> tokens;
};

#include "statement.inl"
