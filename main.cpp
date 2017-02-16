#include <iostream>
#include <fstream>
#include "interpreter.hpp"

int main(int argc, char** argv) {
	std::cout << "hello world" << std::endl;

	std::ifstream inFile("test-statement.txt"); // TODO: not hardcode
	Interpreter terp(inFile);

	terp.parseText();

	inFile.close();

	return 0;
}
