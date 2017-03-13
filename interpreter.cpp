#include "interpreter.hpp"
#include <iostream>

namespace {
	void printAllChildren(Statement* st, int depth = 0) {
		auto it = std::begin(st->getChildren());
		auto end = std::end(st->getChildren());

		while (it != end) {
			std::cout << "|" << Statement::typeAsString((*it)->getType()) << ":\t";

			for (int i = 0; i < depth; ++i) {
				std::cout << "\t";
			}


			for (auto tStart = std::begin((*it)->getTokens()); tStart != std::end((*it)->getTokens()); ++tStart) {
				if (tStart->getTokenType() == Token::TokenType::LINK) {
					std::cout << "[LINK->" << Statement::typeAsString(tStart->getLink()->getType())  << "], ";
				}
				else {
					std::cout << "[" << tStart->getContent() << "]" << ", ";
				}
			}

			std::cout << std::endl;

			printAllChildren(*it, depth + 1);

			++it;
		}
	}

	void printAllTokens(TokenStream& tokenStream) {
		while (tokenStream.canGet()) {
			std::cout << "[" << tokenStream.get().getContent() << "]" << std::endl;
		}
	}
};

Interpreter::Interpreter(std::istream& textStream)
: lexer(std::make_unique<Lexer>(textStream, *this)),
parser(std::make_unique<Parser>(*this)), canContinue(true) {}

void Interpreter::interpretNextStatement() {
	if (currStatement == currEnd) {
		canContinue = false;
		std::cout << "Finished interpreting" << std::endl;
		return; // TODO: check parent or check current scope to see if actually finished parsing
	}

	Statement* stmnt = *currStatement;

	if (stmnt->getType() == Statement::StatementType::VAR_DECLARATION) {
		interpretVarDecl(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::VAR_ASSIGNMENT) {
		interpretVarAssignment(stmnt);
	}
	else {
		std::cout << "skipping " << Statement::typeAsString(stmnt->getType()) << std::endl;
	}

	++currStatement;
}

bool Interpreter::canInterpretStatement() const {
	return canContinue;
}

void Interpreter::parseText() {
	lexAllTokens();

	if (canContinue) {
		parseAllStatements();
	}

	printAllChildren(parser->root);

	parser->currRoot = parser->root;
	
	currStatement = std::begin(parser->currRoot->getChildren());
	currEnd = std::end(parser->currRoot->getChildren());
}

void Interpreter::evalExpression(Statement* expression, std::shared_ptr<Variable> var) {

}

void Interpreter::interpretVarDecl(Statement* statement) {
	std::string varName = statement->getTokens()[0].getContent();
	Statement* expression = statement->getTokens()[1].getLink();

	std::shared_ptr<Variable> var = std::make_shared<Variable>();
	variableMap.emplace(varName, var);

	evalExpression(expression, var);
}

void Interpreter::interpretVarAssignment(Statement* statement) {
	std::string varName = statement->getTokens()[0].getContent();

	std::cout << "setting var " << varName << std::endl;
}

inline void Interpreter::lexAllTokens() {
	while (lexer->canConsumeToken()) {
		lexer->consumeNextToken();
	}

	if (lexer->hasErrored()) {
		canContinue = false;
	}
}

inline void Interpreter::parseAllStatements() {
	while (parser->canConsumeStatement()) {
		parser->consumeNextStatement();
	}

	if (parser->hasErrored()) {
		canContinue = false;
	}
}
