#include <iostream>
#include <fstream>
#include "interpreter.hpp"

void add(std::vector<std::shared_ptr<Variable>>& args, std::shared_ptr<Variable> out) {
	out->type = Variable::VariableType::INT;
	out->intValue = args[0]->intValue - args[1]->intValue;

	for (auto it = std::begin(args), end = std::end(args);
			it != end; ++it) {
		std::cout << Variable::toString(*(*it)) << std::endl;
	}
}

int main(int argc, char** argv) {
	std::ifstream inFile("test-statement.txt"); // TODO: not hardcode
	Interpreter terp(inFile);

	terp.parseText();
	inFile.close();

	terp.addExternalFunc("add", add);

	while (terp.isRunning()) {
		if (terp.hasScriptEvents()) {
			terp.processScript();
		}
	}

	if (terp.getVariable("z") != nullptr) {
		std::cout << "z = " << Variable::toString(*terp.getVariable("z")) << std::endl;
	}
	else {
		std::cout << "z is null" << std::endl;
	}

	return 0;
}
