#pragma once

#include <vector>
#include "token.hpp"

class Statement {
	public:
		enum StatementType {
			VAR_DECLARATION,
			VAR_ASSIGNMENT,
			EXPRESSION,
			FUNC_HEAD,
			GROUPING,
			CONDITIONAL,
			OTHER
		};
		
		Statement(StatementType type = OTHER, Statement* parent = nullptr);

		void addChild(Statement* child);
		void addToken(Token token);

		Statement* getParent();
		std::vector<Statement*>& getChildren();
		std::vector<Token>& getTokens();

		virtual ~Statement() = default;

		static std::string typeAsString(StatementType type);
	private:
		StatementType type;

		Statement* parent;
		std::vector<Statement*> children;
		std::vector<Token> tokens;
};

#include "statement.inl"
