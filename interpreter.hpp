#pragma once

#include <iostream>
#include <memory>
#include "token-stream.hpp"
#include "operator-registry.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "error-log.hpp"

class Lexer;
class Parser;

class Interpreter {
	public:
		Interpreter(std::istream& textStream);

		void parseText();
		void interpretNextStatement();

		bool canInterpretStatement() const;

		virtual ~Interpreter() = default;
	private:
		TokenStream tokenStream;
		OperatorRegistry operatorRegistry;
		ErrorLog errorLog;

		bool canContinue;

		std::unique_ptr<Lexer> lexer;
		std::unique_ptr<Parser> parser;

		void lexAllTokens();
		void parseAllStatements(); // TODO: possibly interpret while parsing

		friend class Lexer;
		friend class Parser;
};
