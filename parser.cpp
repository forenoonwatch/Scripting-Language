/**
 * Big list of things to do:
 *
 * ADD ERROR HANDLING
 * Excise code for parsing scope and put it into its own function
 * possible trim down code required to expect and accept tokens
 *
 */

#include "parser.hpp"
#include "script-error.hpp"

Parser::Parser(Interpreter& interpreter)
: interpreter(interpreter), root(new Statement()), currRoot(root), canParse(true) {}

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
		canParse = false;

		interpreter.errorLog.logError("Syntax error: undefined start to statement:"
			+ nextToken.getContent());
	}
}

bool Parser::canConsumeStatement() const {
	return canParse && interpreter.tokenStream.canGet();
}

bool Parser::hasErrored() const {
	return !canParse;
}

void Parser::consumeVarDeclaration() {
	Statement* varDecl = new Statement(Statement::StatementType::VAR_DECLARATION, currRoot);
	currRoot = varDecl;

	interpreter.tokenStream.get(); // ignore let token

	Token nextToken = interpreter.tokenStream.peek();

	// check and consume variable name
	if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		errorNextToken("identifier");
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get());
	nextToken = interpreter.tokenStream.peek();

	// check and consume operator=
	if (!acceptToken(nextToken, "=")) {
		errorNextToken("=");
		return;
	}

	interpreter.tokenStream.get(); // ignore '='
	
	consumeExpression();

	currRoot = currRoot->getParent(); // go back up from going down in the rhs
}

void Parser::consumeVarAssignment() {
	Statement* varAssign = new Statement(Statement::StatementType::VAR_ASSIGNMENT, currRoot);
	currRoot = varAssign;

	currRoot->addToken(interpreter.tokenStream.get());

	if (!acceptToken(interpreter.tokenStream.peek(), "=")) {
		errorNextToken("=");
		return;
	}

	interpreter.tokenStream.get(); // ignore '='

	consumeExpression();

	currRoot = currRoot->getParent();
}

void Parser::consumeIfStatement() {
	Statement* ifHead = new Statement(Statement::StatementType::CONDITIONAL, currRoot);
	currRoot = ifHead;

	currRoot->addToken(interpreter.tokenStream.get()); // add 'if' token

	consumeExpression();

	if (!acceptToken(interpreter.tokenStream.peek(), "then")) {
		errorNextToken("then");
		return;
	}

	interpreter.tokenStream.get(); // ignore 'then'

	consumeIfScope();

	currRoot = currRoot->getParent();
}

void Parser::consumeIfScope() {
	Token nextToken = interpreter.tokenStream.peek();

	Statement* ifRoot = currRoot;

	while (!acceptToken(nextToken, "end")) {
		if (canConsumeStatement()) {
			if (acceptToken(nextToken, "elif")) {
				consumeElifStatement();
				// create link token to elif and elif root
			}
			else if (acceptToken(nextToken, "else")) {
				// create link token to else and else root
				consumeElseStatement();
			}
			else {
				consumeNextStatement();
			}
		}
		else {
			errorNextToken("statement");
			break;
		}

		nextToken = interpreter.tokenStream.peek();	
	}

	if (!acceptToken(interpreter.tokenStream.peek(), "end")) {
		errorNextToken("end");
		return;
	}

	interpreter.tokenStream.get(); // ignore 'end'
}

void Parser::consumeElifStatement() {
	Statement* ifRoot = currRoot;

	Statement* elifHead = new Statement(Statement::StatementType::CONDITIONAL, currRoot);
	ifRoot->addToken(Token(elifHead));
	currRoot = elifHead;

	currRoot->addToken(interpreter.tokenStream.get()); // add 'elif' token

	consumeExpression();

	if (!acceptToken(interpreter.tokenStream.peek(), "then")) {
		errorNextToken("then");
		return;
	}

	interpreter.tokenStream.get(); // ignore 'then'

	Token nextToken = interpreter.tokenStream.peek();

	while (!acceptToken(nextToken, "end")
			&& !acceptToken(nextToken, "elif") && !acceptToken(nextToken, "else")) {
		if (canConsumeStatement()) {
			consumeNextStatement();
		}
		else {
			errorNextToken("statement");
			break;
		}

		nextToken = interpreter.tokenStream.peek();
	}

	currRoot = ifRoot;
}

void Parser::consumeElseStatement() {
	Statement* ifRoot = currRoot;

	Statement* elseHead = new Statement(Statement::StatementType::CONDITIONAL, currRoot);
	ifRoot->addToken(Token(elseHead));
	currRoot = elseHead;

	currRoot->addToken(interpreter.tokenStream.get()); // add 'else' token

	Token nextToken = interpreter.tokenStream.peek();

	while (!acceptToken(nextToken, "end")
			&& !acceptToken(nextToken, "elif") && !acceptToken(nextToken, "else")) {
		if (canConsumeStatement()) {
			consumeNextStatement();
		}
		else {
			errorNextToken("statement");
			break;
		}

		nextToken = interpreter.tokenStream.peek();
	}

	currRoot = ifRoot;
}

void Parser::consumeWhileLoop() {
	Statement* whileLoop = new Statement(Statement::StatementType::WHILE_LOOP, currRoot);
	currRoot = whileLoop;

	interpreter.tokenStream.get(); // ignore 'while'

	if (acceptToken(interpreter.tokenStream.peek(),
			Token::TokenType::IDENTIFIER | Token::TokenType::NUMERIC | Token::TokenType::STRING)) {
		consumeExpression();
	}
	else {
		errorNextToken("expression");
		return;
	}

	if (!acceptToken(interpreter.tokenStream.peek(), "do")) {
		errorNextToken("do");
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

	if (acceptToken(nextToken, "let")) {
		consumeVarDeclaration();
	}
	else if (acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		consumeVarAssignment();
	}
	else {
		errorNextToken("var assignment or declaration");
		return;
	}

	if (!acceptToken(interpreter.tokenStream.peek(), ",")) {
		errorNextToken(",");
		return;
	}

	interpreter.tokenStream.get(); // ignore ','

	if (interpreter.tokenStream.canGet()) {
		consumeExpression();
	}
	else {
		errorNextToken("expression");
		return;
	}

	nextToken = interpreter.tokenStream.peek();

	if (acceptToken(nextToken, ",")) {
		interpreter.tokenStream.get(); // ignore ','

		if (interpreter.tokenStream.canGet()) {
			consumeExpression();
		}
		else {
			errorNextToken("expression");
			return;
		}

		if (acceptToken(interpreter.tokenStream.peek(), "do")) {
			interpreter.tokenStream.get(); // ignore 'do'
		}
		else {
			errorNextToken("do");
			return;
		}
	}
	else if (acceptToken(nextToken, "do")) {
		interpreter.tokenStream.get(); // ignore 'do'
	}
	else {
		errorNextToken("do");
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
		errorNextToken("identifier");
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get()); // add function name

	if (!acceptToken(interpreter.tokenStream.peek(), "(")) {
		errorNextToken("(");
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

	if (!acceptToken(interpreter.tokenStream.peek(), "(")) {
		errorNextToken("(");
		return;
	}

	consumeParamDeclaration();

	currRoot = currRoot->getParent();
}

void Parser::consumeOtherIdentifier() {
	if (interpreter.tokenStream.canGetNext()) {
		Token nextToken = interpreter.tokenStream.peekNext();

		if (acceptToken(nextToken, "=")) {
			consumeVarAssignment();
		}
		else if (acceptToken(nextToken, "(")) {
			consumeFunctionCall();
		}
		else {
			errorNextToken("function call or var assignment");
		}
	}
	else {
		errorNextToken("function call or var assignment");
	}
}

void Parser::consumeExpression() {
	Token nextToken = interpreter.tokenStream.peek();
	Token::TokenType lastTokenType = Token::TokenType::OTHER;

	Statement* rhs = new Statement(Statement::StatementType::EXPRESSION, currRoot);
	currRoot = rhs;

	int numParens = 0;

	while (interpreter.tokenStream.canGet()) {
		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
			if ((lastTokenType & (Token::TokenType::STRING | Token::TokenType::IDENTIFIER
					| Token::TokenType::NUMERIC)) != 0) {
				// finished parsing
				break;
			}

			if (interpreter.tokenStream.canGetNext() && acceptToken(interpreter.tokenStream.peekNext(), "(")) {
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
		else if (acceptToken(nextToken, "(")) {
			currRoot->addToken(interpreter.tokenStream.get());
			++numParens;
		}
		else if (acceptToken(nextToken, ")")) {
			currRoot->addToken(interpreter.tokenStream.get());
			--numParens;

			if (numParens < 0) {
				interpreter.errorLog.logError("Syntax error: extra )");
				canParse = false;

				break;
			}

			if (interpreter.tokenStream.canGet()
					&& acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER)) {
				break; // finished parsing
			}
		}
		else if (acceptToken(nextToken, ",")) {
			if (currRoot->getParent()->getType() != Statement::StatementType::FUNC_CALL) { // TODO: check for for loop
				errorNextToken("not a comma");
			}

			break;
		}
		else if (acceptToken(nextToken, Token::TokenType::OPERATOR)) {
			currRoot->addToken(interpreter.tokenStream.get());
		}
		else {
			errorNextToken("valid token");
			break;
		}

		lastTokenType = nextToken.getTokenType();
	
	}

	currRoot = currRoot->getParent();	
}

void Parser::consumeFunctionCall() {
	Statement* funcHead = new Statement(Statement::StatementType::FUNC_CALL, currRoot);

	if (currRoot->getType() == Statement::StatementType::EXPRESSION) {
		currRoot->addToken(Token(funcHead));
	}

	currRoot = funcHead;

	Token nextToken = interpreter.tokenStream.peek();

	if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
		errorNextToken("identifier");
		return;
	}

	currRoot->addToken(interpreter.tokenStream.get()); // add func name
	nextToken = interpreter.tokenStream.peek();

	if (!acceptToken(nextToken, "(")) {
		errorNextToken("(");
		return;
	}

	interpreter.tokenStream.get(); // ignore '('

	nextToken = interpreter.tokenStream.peek();

	while (!acceptToken(nextToken, ")")) {
		if (acceptToken(nextToken, Token::TokenType::IDENTIFIER | Token::TokenType::NUMERIC
				| Token::TokenType::STRING)) {
			consumeExpression(); // consume (first) expression
		}
		else if (acceptToken(nextToken, ",")) {
			interpreter.tokenStream.get(); // ignore ','
			
			if (interpreter.tokenStream.canGet()) {
				consumeExpression();
			}
			else {
				errorNextToken("expression");
				return;
			}
		}
		else {
			errorNextToken("function argument");
			return;
		}

		nextToken = interpreter.tokenStream.peek();
	}

	if (!acceptToken(nextToken, ")")) {
		errorNextToken(")");
		return;
	}

	interpreter.tokenStream.get(); // ignore ')'

	currRoot = funcHead->getParent();
}

void Parser::consumeParamDeclaration() {
	Statement* paramDecl = new Statement(Statement::StatementType::GROUPING, currRoot);
	currRoot->addToken(Token(paramDecl));
	currRoot = paramDecl;

	interpreter.tokenStream.get(); // ignore '('

	Token nextToken = interpreter.tokenStream.peek();

	while (interpreter.tokenStream.canGet() && !acceptToken(nextToken, ")")) {
		if (!acceptToken(nextToken, Token::TokenType::IDENTIFIER)) {
			errorNextToken("identifier");
			return;
		}

		currRoot->addToken(interpreter.tokenStream.get());

		nextToken = interpreter.tokenStream.peek();

		if (acceptToken(nextToken, ",")) {
			interpreter.tokenStream.get(); // ignore ','
			
			if (!acceptToken(interpreter.tokenStream.peek(), Token::TokenType::IDENTIFIER)) {
				errorNextToken("identifier");
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

	while (!acceptToken(nextToken, "end")) {
		if (canConsumeStatement()) {
			consumeNextStatement();
		}
		else {
			canParse = false;
			break;
		}

		nextToken = interpreter.tokenStream.peek();	
	}

	if (!acceptToken(interpreter.tokenStream.peek(), "end")) {
		errorNextToken("end");
		return;
	}

	interpreter.tokenStream.get(); // ignore 'end'
}

bool Parser::acceptToken(const Token& token, unsigned type) const {
	if (!canParse || !interpreter.tokenStream.canGet()) {
		return false;
	}

	return token.getTokenType() & type;
}

bool Parser::acceptToken(const Token& token, const std::string& content) const {
	if (!canParse || !interpreter.tokenStream.canGet()) {
		return false;
	}

	return token.getContent().compare(content) == 0;
}

void Parser::errorNextToken(const std::string& expected) {
	canParse = false;
	std::string got;

	if (interpreter.tokenStream.canGet()) {
		got = interpreter.tokenStream.peek().getContent();
	}
	else {
		got = "EOF";
	}

	interpreter.errorLog.logError(SyntaxError(expected, got));
}
