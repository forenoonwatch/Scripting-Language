#include "interpreter.hpp"

Interpreter::Interpreter(std::istream& textStream)
: lexer(std::make_unique<Lexer>(textStream, *this)) {}

void Interpreter::parseText() {
	while (lexer->canConsumeToken()) {
		lexer->consumeNextToken();
	}
}