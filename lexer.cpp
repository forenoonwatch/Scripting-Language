#include "lexer.hpp"

Lexer::Lexer(std::istream& textStream, TokenStream& tokenStream)
: textStream(textStream), tokenStream(tokenStream) {}

void Lexer::consumeNextToken() {
	char nextChar = textStream.peek();

	if (isWhitespace(nextChar)) {
		consumeWhitespace();
	}
	else if (isIdentifier(nextChar)) {
		consumeIdentifier();
	}
	else if (isNumericLiteral(nextChar)) {
		consumeNumericLiteral();
	}
	else if (isStringLiteral(nextChar)) {
		consumeStringLiteral();
	}
	else {
		consumeOperator(nextChar);
	}
}

bool Lexer::canConsumeToken() const {
	return !textStream.eof();
}
