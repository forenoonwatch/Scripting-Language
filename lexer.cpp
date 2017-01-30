#include "lexer.hpp"

Lexer::Lexer(std::istream& textStream, TokenStream& tokenStream)
: textStream(textStream), tokenStream(tokenStream) {}

void Lexer::consumeNextToken() {}
