#include "parser.hpp"

Parser::Parser(Interpreter& interpreter)
: interpreter(interpreter), currRoot(root) {}

void Parser::consumeNextStatement() {
	Token nextToken = interpreter.tokenStream.peek();

	if (nextToken.getTokenType() == Token::TokenType::IDENTIFIER) {
		if (nextToken.getContent().compare("let") == 0) {
			consumeVarDeclaration();
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
	Statement varDecl(Statement::StatementType::VAR_DECLARATION, currRoot);
	currRoot = varDecl;

	interpeter.tokenStream.get(); // ignore let token

	Token nextToken = interpreter.tokenStream.peek();

	if (!acceptToken(nextToken, Token::TokenType::OPERATOR, "=")) {
		return;
	}

	currRootl.addToken(interpreter.tokenStream.get());

	while (tokenStream.canGet()) {
		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
			varDecl.addToken(interpreter.tokenStream.get());
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, "(")) {
			currRoot = Statement(Statement::StatementType::OTHER, currRoot);
			currRoot.addToken(interpreter.tokenStream.get());
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, ")")) {
			if (currRoot != varDecl) {
				currRoot.addToken(interpreter.tokenStream.get());
				currRoot = currRoot.getParent();
			}
			else {
				// error: extra )
			}
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR)) {
			varDecl.addToken(interpreter.tokenStream.get());
		}
		else {
			// error: invalid token
		}
	}

	currRoot = varDecl; // go back up from going down in the rhs
}

bool Parser::acceptToken(const Token& token, Token::TokenType type) {
	if (!tokenStream.canGet()) {
		return false;
	}

	if (token.getTokenType() != type) {
		std::cerr << "Error: token type mismatch on " << token.getContent() << std::endl;
		return false;
	}

	return true;
}

bool Parser::acceptToken(const Token& token, Token::TokenType type, const std::string& content) {
	if (!tokenStream.canGet()) {
		return false;
	}

	if (token.getTokenType() != type || token.compare(content) != 0) {
		std::cerr << "Error: expected " << content << ", got " << token.getContent() << std::endl;
		return false;
	}

	return true;
}
