#pragma once

#include <iostream>
#include <memory>
#include "token-stream.hpp"
#include "operator-registry.hpp"
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
		OperatorRegistry operatorRegistry;
		std::unique_ptr<Lexer> lexer;

		friend class Lexer;
};

#include "interpreter.inl"
