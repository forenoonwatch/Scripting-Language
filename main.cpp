#include <iostream>
#include <fstream>
#include "interpreter.hpp"

int main(int argc, char** argv) {
	std::ifstream inFile("test-statement.txt"); // TODO: not hardcode
	Interpreter terp(inFile);

	terp.parseText();
	inFile.close();

	while (terp.canInterpretStatement()) {
		terp.interpretNextStatement();
	}

	return 0;
}
