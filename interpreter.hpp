#pragma once

#include <iostream>
#include <memory>
#include "token-stream.hpp"
#include "lexer.hpp"

class Lexer;

class Interpreter {
	public:
		Interpreter(std::istream& textStream);

		void parseText();

		TokenStream& getTokenStream();

		virtual ~Interpreter() = default;
	private:
		TokenStream tokenStream;
		std::unique_ptr<Lexer> lexer;
};

#include "interpreter.inl"
