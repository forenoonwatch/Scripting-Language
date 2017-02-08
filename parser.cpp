#include "parser.hpp"

Parser::Parser(Interpreter& interpreter)
: interpreter(interpreter), currRoot(root) {}

void Parser::consumeNextStatement() {
	Token nextToken = interpreter.tokenStream.peek();

	if (nextToken.getTokenType() == Token::TokenType::IDENTIFIER) {
		if (nextToken.getContent().compare("let") == 0) {
			std::cout << "parse var declaration" << std::endl;
		}
		else if (nextToken.getContent().compare("if") == 0) {
			std::cout << "parse if statement" << std::endl;
		}
		else if (nextToken.getContent().compare("fun") == 0) {
			std::cout << "parse function declaration" << std::endl;
		}
	}
	else {
		std::cout << "error?" << std::endl;
	}
}

void Parser::consumeVarDeclaration() {
	Statement varDecl(Statement::StatementType::VAR_DECLARATION);
}
