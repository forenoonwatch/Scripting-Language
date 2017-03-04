/**
 * Big list of things to do:
 *
 * ADD ERROR HANDLING
 * Excise code for parsing scope and put it into its own function
 * possible trim down code required to expect and accept tokens
 * Possibly remove need to compare token types when comparing token content
 *
 */

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
			consumeWhileLoop();
		}
		else if (nextToken.getContent().compare("for") == 0) {
			consumeForLoop();
		}
		else {
			consumeOtherIdentifier();
		}
	}
	else {
		//std::cout << "error: statement start is not an identifier" << std::endl;
	}
}

bool Parser::canConsumeStatement() const {
	return interpreter.tokenStream.canGet();
}

void Parser::consumeVarDeclaration() {
	Statement* varDecl = new Statement(Statement::StatementType::VAR_DECLARATION, currRoot);
	currRoot = varDecl;

	interpreter.tokenStream.get(); // ignore let token

	Token nextToken = interpreter.tokenStream.peek();

	// check and consume variable name
	if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		std::cerr << "unexpected token in var decl: " << nextToken.getContent() << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());
	nextToken = interpreter.tokenStream.peek();

	// check and consume operator=
	if (!acceptToken(nextToken, Token::TokenType::OPERATOR, "=")) {
		std::cerr << "unexpected token in var decl" << nextToken.getContent() << std::endl;
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
		std::cerr << "unexpected token in var assign: "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
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
		std::cerr << "unexpected token: expected 'then' got "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	interpreter.tokenStream.get(); // ignore 'then'

	consumeScopeBlock();

	currRoot = currRoot->getParent();
}

void Parser::consumeWhileLoop() {
	Statement* whileLoop = new Statement(Statement::StatementType::WHILE_LOOP, currRoot);
	currRoot = whileLoop;

	interpreter.tokenStream.get(); // ignore 'while'

	if (acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER)) {
		consumeExpression();
	}
	else {
		std::cout << "unexpected token: expected identifier got "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER, "do")) {
		std::cout << "unexpected token: expected 'do' got "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	interpreter.tokenStream.get(); // ignore 'do'

	consumeScopeBlock();

	currRoot = currRoot->getParent();
}

void Parser::consumeForLoop() {
	Statement* forLoop = new Statement(Statement::StatementType::FOR_LOOP, currRoot);
	currRoot = forLoop;

	interpreter.tokenStream.get(); // ignore 'for'

	Token nextToken = interpreter.tokenStream.peek();

	if (acceptToken(nextToken, Token::TokenType::IDENTIFIER, "let")) {
		consumeVarDeclaration();
	}
	else if (acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		consumeVarAssignment();
	}
	else {
		std::cerr << "Invalid token: Expected var assignment or declaration" << std::endl;
		return;
	}

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::OPERATOR, ",")) {
		std::cerr << "Invalid token: expected ',' got "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	interpreter.tokenStream.get(); // ignore ','

	if (interpreter.tokenStream.canGet()) {
		consumeExpression();
	}
	else {
		std::cerr << "Reached EOF too early" << std::endl;
		return;
	}

	nextToken = interpreter.tokenStream.peek();

	if (acceptToken(nextToken, Token::TokenType::OPERATOR, ",")) {
		interpreter.tokenStream.get(); // ignore ','

		if (interpreter.tokenStream.canGet()) {
			consumeExpression();
		}
		else {
			std::cerr << "Reached EOF too early" << std::endl;
			return;
		}

		if (acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER, "do")) {
			interpreter.tokenStream.get(); // ignore 'do'
		}
		else {
			std::cerr << "invalid token: expected 'do' got "
				<< interpreter.tokenStream.peek().getContent() << std::endl;
		}
	}
	else if (acceptToken(nextToken, Token::TokenType::IDENTIFIER, "do")) {
		interpreter.tokenStream.get(); // ignore 'do'
	}
	else {
		std::cerr << "Invalid token: expected 'do' or expression, got "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	consumeScopeBlock();

	currRoot = currRoot->getParent();
}

void Parser::consumeFunDeclaration() {
	Statement* funDecl = new Statement(Statement::StatementType::FUNC_DECL, currRoot);
	currRoot = funDecl;

	interpreter.tokenStream.get(); // ignore 'fun'

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER)) {
		std::cerr << "unexpected token in fun decl: "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get()); // add function name

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::OPERATOR, "(")) {
		std::cerr << "unexpected token in fun decl: expected '(' got "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	consumeParamDeclaration();

	consumeScopeBlock();

	currRoot = currRoot->getParent();
}

void Parser::consumeAnonymousFunDecl() {
	Statement* funDecl = new Statement(Statement::StatementType::FUNC_DECL, currRoot);
	currRoot = funDecl;

	interpreter.tokenStream.get(); // ignore 'fun'

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::OPERATOR, "(")) {
		std::cerr << "unexpected token in anon fun decl: expected '(' got "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	consumeParamDeclaration();

	currRoot = currRoot->getParent();
}

void Parser::consumeOtherIdentifier() {
	if (interpreter.tokenStream.canGetNext()) {
		Token nextToken = interpreter.tokenStream.peekNext();

		if (acceptToken(nextToken, Token::TokenType::OPERATOR, "=")) {
			consumeVarAssignment();
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, "(")) {
			consumeFunctionCall();
		}
		else {
			std::cerr << "Invalid statement: got " << 
			   interpreter.tokenStream.peek().getContent() << std::endl;
		}
	}
	else {
		std::cerr << "Not enough tokens left" << std::endl;
	}
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

		if (acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
			if ((lastTokenType & (Token::TokenType::STRING | Token::TokenType::IDENTIFIER
					| Token::TokenType::NUMERIC)) != 0) {
				// finished parsing
				break;
			}

			if (interpreter.tokenStream.canGetNext() && acceptToken(interpreter.tokenStream.peekNext(),
					Token::TokenType::OPERATOR, "(")) {
				consumeFunctionCall();
			}
			else {
				currRoot->addToken(interpreter.tokenStream.get());
			}
		}
		else if (acceptToken(nextToken, Token::TokenType::STRING | Token::TokenType::NUMERIC)) {
			if ((lastTokenType & (Token::TokenType::STRING
					| Token::TokenType::IDENTIFIER | Token::TokenType::NUMERIC)) != 0) {
				// finished parsing
				break;
			}

			currRoot->addToken(interpreter.tokenStream.get());
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, "(")) {
			interpreter.tokenStream.get();

			Statement* paren = new Statement(Statement::StatementType::GROUPING, currRoot);
			currRoot->addToken(Token(paren));

			currRoot = paren;
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, ")")) {
			if (currRoot->getParent() != baseRoot) {
				interpreter.tokenStream.get();
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
				if (currRoot->getParent()->getType() == Statement::StatementType::FUNC_CALL
						|| currRoot->getParent()->getParent()->getType() == Statement::StatementType::FOR_LOOP) {
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

	if (currRoot->getType() == Statement::StatementType::EXPRESSION) {
		currRoot->addToken(Token(funcHead));
	}

	currRoot = funcHead;

	Token nextToken = interpreter.tokenStream.peek();

	if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		std::cerr << "Error: expected identifier for function head" << std::endl;
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get()); // add func name
	nextToken = interpreter.tokenStream.peek();

	if (!acceptToken(nextToken, Token::TokenType::OPERATOR, "(")) {
		std::cerr << "Error: expected ( after identifier" << std::endl;
		return;
	}

	interpreter.tokenStream.get(); // ignore '('

	while (interpreter.tokenStream.canGet()) {
		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken, Token::TokenType::OPERATOR, ")")) {
			interpreter.tokenStream.get(); // end func call parse
			break;
		}
		else if (acceptToken(nextToken, Token::TokenType::IDENTIFIER | Token::TokenType::NUMERIC
				| Token::TokenType::STRING)) {
			consumeExpression(); // consume (first) expression
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR, ",")) {
			interpreter.tokenStream.get(); // ignore ','
			
			if (interpreter.tokenStream.canGet()) {
				consumeExpression();
			}
			else {
				std::cerr << "met EOF too early in func call" << std::endl;
			}
		}
		else {
			std::cerr << "unexpected token in function call: " << nextToken.getContent() << std::endl;
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

	while (interpreter.tokenStream.canGet() && !acceptToken(nextToken, Token::TokenType::OPERATOR, ")")) {
		if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
			std::cerr << "unexpected token: expected identifier" << std::endl;
			return;
		}

		currRoot->addToken(interpreter.tokenStream.get());

		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken, Token::TokenType::OPERATOR, ",")) {
			interpreter.tokenStream.get(); // ignore ','
			
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

void Parser::consumeScopeBlock() {
	Token nextToken = interpreter.tokenStream.peek();

	while (!acceptToken(nextToken, Token::TokenType::IDENTIFIER, "end")) {
		consumeNextStatement();
		nextToken = interpreter.tokenStream.peek();	
	}

	if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER, "end")) {
		std::cerr << "unexpected token: expected 'end' got "
			<< interpreter.tokenStream.peek().getContent() << std::endl;
		return;
	}

	interpreter.tokenStream.get(); // ignore 'end'
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
