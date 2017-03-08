#include "lexer.hpp"
#include "script-error.hpp"
#include <sstream>
#include <cctype>

Lexer::Lexer(std::istream& textStream, Interpreter& interpreter)
: textStream(textStream), interpreter(interpreter), canContinue(true) {}

void Lexer::consumeNextToken() {
	char nextChar = textStream.peek();

	if (isWhitespace(nextChar)) {
		consumeWhitespace();
	}
	else if (isIdentifierStart(nextChar)) {
		consumeIdentifier();
	}
	else if (isNumericLiteral(nextChar) && nextChar != 'x') {
		consumeNumericLiteral();
	}
	else if (isStringLiteral(nextChar)) {
		consumeStringLiteral();
	}
	else {
		consumeOperator();
	}
}

bool Lexer::canConsumeToken() const {
	return canContinue && !textStream.eof();
}

bool Lexer::hasErrored() const {
	return !canContinue;
}

void Lexer::consumeWhitespace() {
	char nextChar;
	bool createdEndline = false;

	do {
		nextChar = textStream.get();

		if (!createdEndline && nextChar == '\n') {
			createdEndline = true;
			//interpreter.tokenStream.addToken(";", Token::TokenType::OPERATOR);
		}
	}
	while (canConsumeToken() && isWhitespace(textStream.peek()));
}

void Lexer::consumeIdentifier() {
	std::stringstream content;

	do {
		content << static_cast<char>(textStream.get());
	}
	while (canConsumeToken() && isIdentifier(textStream.peek()));

	interpreter.tokenStream.addToken(content.str(), Token::TokenType::IDENTIFIER);
}

void Lexer::consumeNumericLiteral() {
	std::stringstream content;

	do {
		content << static_cast<char>(textStream.get());
	}
	while (canConsumeToken() && isNumericLiteral(textStream.peek()));

	interpreter.tokenStream.addToken(content.str(), Token::TokenType::NUMERIC);
}

void Lexer::consumeStringLiteral() {
	std::stringstream content;
	char nextChar;

	content << static_cast<char>(textStream.get()); // consume first '"'

	while (canConsumeToken()) {
		nextChar = textStream.get();
		content << nextChar;

		if (nextChar == '\\') { // TODO: possibly auto-resolve escape characters
			if (canConsumeToken()) {
				content << static_cast<char>(textStream.get());
			} 
			else {
				errorNextChar("symbol");
				return;
			}
		}
		else if (nextChar == '\n') {
			errorNextChar("\"");
			return;
		}
		else if (isStringLiteral(nextChar)) {
			break;
		}
	}

	interpreter.tokenStream.addToken(content.str(), Token::TokenType::STRING);
}

void Lexer::consumeOperator() {
	std::stringstream content;
	char nextChar = textStream.get();

	if (interpreter.operatorRegistry.isValidOperatorChar(nextChar)) {
		content << nextChar;
	}
	else {
		errorNextChar("valid operator");
		return;
	}

	do {
		nextChar = textStream.peek();

		if (!interpreter.operatorRegistry.isValidOperator(content.str() + nextChar)) {
			// TODO: check here to make sure current formed token is a valid operator and throw error otherwise

			if (isPossibleOperator(nextChar)) {
				errorNextChar("valid operator symbol");
				return;
			}

			interpreter.tokenStream.addToken(content.str(), Token::TokenType::OPERATOR);
			break;
		}
		else {
			content << static_cast<char>(textStream.get());
		}
	}
	while (interpreter.operatorRegistry.isValidOperatorChar(nextChar));
}

void Lexer::errorNextChar(const std::string& expected) {
	canContinue = false;
	std::string got;

	if (!textStream.eof()) {
		got = std::string(1, static_cast<char>(textStream.peek()));
	}
	else {
		got = "EOF";
	}

	interpreter.errorLog.logError(SyntaxError(expected, got));
}

bool Lexer::isWhitespace(char chr) {
	return isspace(chr);
}

bool Lexer::isIdentifierStart(char chr) {
	return isalpha(chr) || chr == '_';
}

bool Lexer::isIdentifier(char chr) {
	return isalnum(chr) || chr == '_';
}

bool Lexer::isNumericLiteral(char chr) {
	// Hexadecimal support
	return isdigit(chr) || isxdigit(chr) || chr == 'x' || chr == '.';
}

bool Lexer::isStringLiteral(char chr) {
	return chr == '"';
}

bool Lexer::isPossibleOperator(char chr) {
	return ispunct(chr);
}
