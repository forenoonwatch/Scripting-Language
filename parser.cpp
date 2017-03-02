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
			consumeIfStatement();
		}
		else if (nextToken.getContent().compare("fun") == 0) {
			consumeFunDeclaration();
		}
		else if (nextToken.getContent().compare("while") == 0) {
			std::cout << "parse while loop" << std::endl;
		}
		else if (nextToken.getContent().compare("for") == 0) {
			std::cout << "parse for loop" << std::endl;
		}
		else {
			consumeOtherIdentifier();
		}
	}
	else {
		std::cout << "error: statement start is not an identifier" << std::endl;
	}
}

bool Parser::canConsumeStatement() const {
	return interpreter.tokenStream.canGet();
}

void Parser::consumeOtherIdentifier() {
	if (interpreter.tokenStream.canGetNext()) {
		Token nextToken = interpreter.tokenStream.peekNext();

		if (acceptToken(nextToken, Token::TokenType::OPERATOR, "=")) {
			consumeVarAssignment();
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, "(")) {
			consumeFunDeclaration();
		}
		else {
			std::cerr << "Invalid statement" << std::endl;
		}
	}
	else {
		std::cerr << "Not enough tokens left" << std::endl;
	}
}

void Parser::consumeVarDeclaration() {
	Statement* varDecl = new Statement(Statement::StatementType::VAR_DECLARATION, currRoot);
	currRoot = varDecl;

	interpreter.tokenStream.get(); // ignore let token

	Token nextToken = interpreter.tokenStream.peek();

	// check and consume variable name
	if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());
	nextToken = interpreter.tokenStream.peek();

	// check and consume operator=
	if (!acceptToken(nextToken, Token::TokenType::OPERATOR, "=")) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());
	
	consumeExpression();

	currRoot = currRoot->getParent(); // go back up from going down in the rhs
}

void Parser::consumeVarAssignment() {
	Statement* varAssign = new Statement(Statement::StatementType::VAR_ASSIGNMENT, currRoot);
	currRoot = varAssign;

	currRoot->addToken(interpreter.tokenStream.get());

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::OPERATOR, "=")) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());

	consumeExpression();

	currRoot = currRoot->getParent();
}

void Parser::consumeIfStatement() {
	Statement* ifHead = new Statement(Statement::StatementType::CONDITIONAL, currRoot);
	currRoot = ifHead;

	interpreter.tokenStream.get(); // ignore 'if'

	consumeExpression();

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER, "then")) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	interpreter.tokenStream.get(); // ignore 'then'

	Token nextToken = interpreter.tokenStream.peek();

	while (nextToken.getContent() != "end") {
		consumeNextStatement();
		nextToken = interpreter.tokenStream.peek();	
	}

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER, "end")) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	interpreter.tokenStream.get(); // ignore 'end'

	currRoot = currRoot->getParent();
}

void Parser::consumeFunDeclaration() {
	Statement* funDecl = new Statement(Statement::StatementType::FUNC_DECL, currRoot);
	currRoot = funDecl;

	interpreter.tokenStream.get(); // ignore 'fun'

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER)) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get()); // add function name

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::OPERATOR, "(")) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	consumeParamDeclaration();

	Token nextToken = interpreter.tokenStream.peek();

	while (nextToken.getContent() != "end") {
		consumeNextStatement();
		nextToken = interpreter.tokenStream.peek();	
	}

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER, "end")) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	interpreter.tokenStream.get(); // ignore 'end'

	currRoot = currRoot->getParent();
}

void Parser::consumeAnonymousFunDecl() {
	Statement* funDecl = new Statement(Statement::StatementType::FUNC_DECL, currRoot);
	currRoot = funDecl;

	interpreter.tokenStream.get(); // ignore 'fun'

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::OPERATOR, "(")) {
		std::cerr << "unexpected token" << std::endl;
		return;
	}

	consumeParamDeclaration();

	currRoot = currRoot->getParent();
}

void Parser::consumeExpression() {
	Token nextToken = interpreter.tokenStream.peek();
	Token::TokenType lastTokenType = Token::TokenType::OTHER;

	Statement* baseRoot = currRoot;

	Statement* rhs = new Statement(Statement::StatementType::EXPRESSION, currRoot);
	currRoot->addToken(Token(rhs));
	currRoot = rhs;

	while (interpreter.tokenStream.canGet()) {
		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken,
				Token::TokenType::IDENTIFIER | Token::TokenType::NUMERIC | Token::TokenType::STRING)) {
			if (interpreter.tokenStream.canGetNext()
					&& acceptToken(interpreter.tokenStream.peekNext(), Token::TokenType::OPERATOR, "(")) {
				consumeFunctionCall();
			}
			else {
				if (lastTokenType & (Token::TokenType::IDENTIFIER
						| Token::TokenType::NUMERIC | Token::TokenType::STRING)) {
					// finished parsing
					break;
				}
				else {
					currRoot->addToken(interpreter.tokenStream.get());
				}
			}
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, "(")) {
			interpreter.tokenStream.get();

			Statement* paren = new Statement(Statement::StatementType::GROUPING, currRoot);
			currRoot->addToken(Token(paren));

			currRoot = paren;
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, ")")) {
			if (currRoot != baseRoot) {
				interpreter.tokenStream.get();
				//currRoot->addToken(interpreter.tokenStream.get());
				currRoot = currRoot->getParent();
			}
			else {
				// error: extra )
				//std::cerr << "ERROR: extra )" << std::endl;
				break;
			}
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR)) {
			if (nextToken.getContent().compare(",") == 0) {
				if (currRoot->getParent()->getType() == Statement::StatementType::FUNC_CALL) {
					break; // TODO: see if this doesn't break anything
				}
				else {
					std::cerr << "ERROR: Incorrect usage of comma!" << std::endl;
					break;
				}
			}
			else {
				currRoot->addToken(interpreter.tokenStream.get());
			}
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
	currRoot->addToken(Token(funcHead));
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

void Parser::consumeParamDeclaration() {
	Statement* paramDecl = new Statement(Statement::StatementType::GROUPING, currRoot);
	currRoot->addToken(Token(paramDecl));
	currRoot = paramDecl;

	interpreter.tokenStream.get(); // ignore '('

	Token nextToken = interpreter.tokenStream.peek();

	while (!acceptToken(nextToken, Token::TokenType::OPERATOR, ")")) {
		if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
			std::cerr << "unexpected token: expected identifier" << std::endl;
			return;
		}

		currRoot->addToken(interpreter.tokenStream.get());

		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken, Token::TokenType::OPERATOR, ",")) {
			currRoot->addToken(interpreter.tokenStream.get()); // TODO: possibly omit commas
			
			if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER)) {
				std::cerr << "unexpected token: expected identifier" << std::endl;
				return;
			}

			currRoot->addToken(interpreter.tokenStream.get());

			nextToken = interpreter.tokenStream.peek();
		}
	}

	interpreter.tokenStream.get(); // ignore ')';	

	currRoot = currRoot->getParent();
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
