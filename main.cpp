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

		std::shared_ptr<Variable> var = terp.getVariable("x");

		if (var != nullptr) {
			std::cout << var->intValue << std::endl;
		}
	}

	//std::cout << "x = " << (terp.getVariable("x")->boolValue ? "true" : "false") << std::endl;
	//std::cout << "y = " << terp.getVariable("y")->intValue << std::endl;

	return 0;
}
