#include "interpreter.hpp"
#include <iostream>

namespace {
	void printAllChildren(Statement* st, int depth = 0) {
		auto it = std::begin(st->getChildren());
		auto end = std::end(st->getChildren());

		while (it != end) {
			std::cout << "|" << Statement::typeAsString((*it)->getType()) << ": ";

			for (int i = 0; i < depth; ++i) {
				std::cout << "\t";
			}


			for (auto tStart = std::begin((*it)->getTokens()); tStart != std::end((*it)->getTokens()); ++tStart) {
				if (tStart->getTokenType() == Token::TokenType::LINK) {
					std::cout << "[LINK->" << Statement::typeAsString(tStart->getLink()->getType())  << "], ";
				}
				else {
					std::cout << "[" << tStart->getContent() << "]" << ", ";
				}
			}

			std::cout << std::endl;

			printAllChildren(*it, depth + 1);

			++it;
		}
	}
};

Interpreter::Interpreter(std::istream& textStream)
: lexer(std::make_unique<Lexer>(textStream, *this)),
parser(std::make_unique<Parser>(*this)) {}

void Interpreter::parseText() {
	while (lexer->canConsumeToken()) {
		lexer->consumeNextToken();
	}

	while (parser->canConsumeStatement()) {
		parser->consumeNextStatement();
	}

	//while (tokenStream.canGet()) {
		//std::cout << "[" << tokenStream.get().getContent() << "]" << std::endl;
	//}

	std::cout << parser->root->getChildren().size() << std::endl;

	printAllChildren(parser->root);
}
