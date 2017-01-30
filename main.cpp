#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "token-stream.hpp"

int main(int argc, char** argv) {
	std::cout << "hello world" << std::endl;

	std::ifstream inFile("script.txt"); // TODO: not hardcode
	TokenStream tokenStream;

	Lexer lexer(inFile, tokenStream);

	while (lexer.canConsumeToken()) {
		lexer.consumeNextToken();
	}

	inFile.close();

	return 0;
}
