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

	//std::cout << "x = " << (terp.getVariable("x")->boolValue ? "true" : "false") << std::endl;

	if (terp.getVariable("z") != nullptr) {
		std::cout << "z = " << Variable::toString(*terp.getVariable("z")) << std::endl;
	}
	else {
		std::cout << "z is null" << std::endl;
	}

	return 0;
}
