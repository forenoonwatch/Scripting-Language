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
	do {
		textStream.get(); // pass over whitespace in stream
	}
	while (canConsumeToken() && isIdentifier(textStream.peek()));
}

void Lexer::consumeIdentifier() {
	std::stringstream content;

	do {
		content << static_cast<char>(textStream.get());
	}
	while (canConsumeToken() && isIdentifier(textStream.peek()));

	interpreter.getTokenStream().addToken(content.str(), Token::TokenType::IDENTIFIER);
}

void Lexer::consumeNumericLiteral() {
	std::stringstream content;

	do {
		content << static_cast<char>(textStream.get());
	}
	while (canConsumeToken() && isNumericLiteral(textStream.peek()));

	interpreter.getTokenStream().addToken(content.str(), Token::TokenType::NUMERIC);
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

	interpreter.getTokenStream().addToken(content.str(), Token::TokenType::STRING);
}

void Lexer::consumeOperator() {
	// TODO: implement this method
	textStream.get();
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
	return false;
}
