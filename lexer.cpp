#include "lexer.hpp"
#include <sstream>
#include <cctype>

Lexer::Lexer(std::istream& textStream, Interpreter& interpreter)
: textStream(textStream), interpreter(interpreter) {}

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
	return !textStream.eof();
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
			} // TODO: throw syntax error for unexpected end to character
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
		// TODO: throw error for invalid operator
	}

	do {
		nextChar = textStream.peek();

		if (!interpreter.operatorRegistry.isValidOperator(content.str() + nextChar)) {
			// TODO: check here to make sure current formed token is a valid operator and throw error otherwise
			interpreter.tokenStream.addToken(content.str(), Token::TokenType::OPERATOR);
			break;
		}
		else {
			content << static_cast<char>(textStream.get());
		}
	}
	while (interpreter.operatorRegistry.isValidOperatorChar(nextChar));
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
