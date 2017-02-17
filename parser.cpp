#include "parser.hpp"

Parser::Parser(Interpreter& interpreter)
: interpreter(interpreter), root(new Statement()), currRoot(root) {}

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
	Statement* varDecl = new Statement(Statement::StatementType::VAR_DECLARATION, currRoot);
	currRoot = varDecl;

	interpreter.tokenStream.get(); // ignore let token

	Token nextToken = interpreter.tokenStream.peek();

	// check and consume variable name
	if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());
	nextToken = interpreter.tokenStream.peek();

	// check and consume operator=
	if (!acceptToken(nextToken, Token::TokenType::OPERATOR, "=")) {
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());

	// begin parsing rhs; TODO: move to consumeExpression()
	consumeExpression();

	currRoot = varDecl; // go back up from going down in the rhs
}

void Parser::consumeExpression() {
	Token nextToken = interpreter.tokenStream.peek();
	Token::TokenType lastTokenType = Token::TokenType::OTHER;

	Statement* baseRoot = currRoot;

	while (interpreter.tokenStream.canGet()) {
		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken,
				Token::TokenType::IDENTIFIER | Token::TokenType::NUMERIC | Token::TokenType::STRING)) {
			if (interpreter.tokenStream.canGetNext()
					&& acceptToken(interpreter.tokenStream.peekNext(), Token::TokenType::OPERATOR, "(")) {
				currRoot->addToken(nextToken);
				consumeFunctionCall();
			}
			else {
				if (lastTokenType & (Token::TokenType::IDENTIFIER
						| Token::TokenType::NUMERIC | Token::TokenType::STRING)) {
					// finished parsing
					std::cout << "last token type: " << lastTokenType << std::endl;
					break;
				}
				else {
					currRoot->addToken(interpreter.tokenStream.get());
				}
			}
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, "(")) {
			currRoot->addToken(interpreter.tokenStream.get());

			Statement* paren = new Statement(Statement::StatementType::GROUPING, currRoot);
			currRoot = paren;
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, ")")) {
			if (currRoot != baseRoot) {
				currRoot->addToken(interpreter.tokenStream.get());
				currRoot = currRoot->getParent();
			}
			else {
				// error: extra )
				//std::cerr << "ERROR: extra )" << std::endl;
				break;
			}
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR)) {
			currRoot->addToken(interpreter.tokenStream.get());
		}
		else {
			// error: invalid token
			std::cerr << "ERROR: invalid token" << std::endl;
			break;
		}

		lastTokenType = nextToken.getTokenType();
	}

	currRoot = baseRoot;
}

void Parser::consumeFunctionCall() {
	Statement* funcHead = new Statement(Statement::StatementType::FUNC_CALL, currRoot);
	currRoot = funcHead;

	Token nextToken = interpreter.tokenStream.peek();

	if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		std::cerr << "Error: expected identifier for function head" << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());
	nextToken = interpreter.tokenStream.peek();

	if (!acceptToken(nextToken, Token::TokenType::OPERATOR, "(")) {
		std::cerr << "Error: expected ( after identifier" << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());

	while (interpreter.tokenStream.canGet()) {
		consumeExpression();
		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken, Token::TokenType::OPERATOR, ",")) {
			currRoot->addToken(interpreter.tokenStream.get()); // continue onward
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, ")")) {
			currRoot->addToken(interpreter.tokenStream.get());
			break; // finish function parse
		}
		else {
			std::cerr << "unexpected token in function call" << std::endl;
			return;
		}
	}

	currRoot = funcHead->getParent();
}

bool Parser::acceptToken(const Token& token, unsigned type) {
	if (!interpreter.tokenStream.canGet()) {
		return false;
	}

	return token.getTokenType() & type;
}

bool Parser::acceptToken(const Token& token, Token::TokenType type, const std::string& content) {
	if (!interpreter.tokenStream.canGet()) {
		return false;
	}

	return token.getTokenType() == type && token.getContent().compare(content) == 0;
}
