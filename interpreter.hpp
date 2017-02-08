#pragma once

#include <iostream>
#include <memory>
#include "token-stream.hpp"
#include "operator-registry.hpp"
#include "lexer.hpp"
#include "parser.hpp"

class Lexer;
class Parser;

class Interpreter {
	public:
		Interpreter(std::istream& textStream);

		void parseText();

		TokenStream& getTokenStream();

		virtual ~Interpreter() = default;
	private:
		TokenStream tokenStream;
		OperatorRegistry operatorRegistry;
		std::unique_ptr<Lexer> lexer;
		std::unique_ptr<Parser> parser;

		friend class Lexer;
		friend class Parser;
};

#include "interpreter.inl"
