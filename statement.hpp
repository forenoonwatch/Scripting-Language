#pragma once

#include <memory>
#include <vector>
#include "token.hpp"

class Statement {
	public:
		enum StatementType {
			VAR_DECLARATION,
			VAR_ASSIGNMENT,
			CONDITIONAL,
			OTHER
		};
		
		Statement(StatementType type = OTHER, std::shared_ptr<Statement> parent = nullptr);

		void addChild(std::shared_ptr<Statement> child);
		void addToken(Token token);

		Statement& getParent();

		virtual ~Statement() = default;
	private:
		StatementType type;

		std::shared_ptr<Statement> parent;
		std::vector<std::shared_ptr<Statement>> children;
		std::vector<Token> tokens;
};

#include "statement.inl"
