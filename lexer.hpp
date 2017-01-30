#pragma once

#include <iostream>
#include "token.hpp"
#include "token-stream.hpp"

class Lexer {
	public:
		Lexer(std::istream& textStream, TokenStream& tokenStream);

		void consumeNextToken();

		virtual ~Lexer() = default;
	private:
		std::istream& textStream;
		TokenStream& tokenStream;
};
