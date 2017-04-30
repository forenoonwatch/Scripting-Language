#include <iostream>
#include <fstream>
#include <chrono>
#include "interpreter.hpp"

namespace Time {
	std::chrono::high_resolution_clock::time_point start
		= std::chrono::high_resolution_clock::now();

	double getTime() {
		using namespace std::chrono;
		return duration_cast<duration<double>>(high_resolution_clock::now()
			- start).count();
	}
};

std::unique_ptr<Interpreter> terp;
double lastYield;

void add(std::vector<std::shared_ptr<Variable>>& args, std::shared_ptr<Variable> out) {
	out->type = Variable::VariableType::INT;
	out->intValue = args[0]->intValue - args[1]->intValue;

	for (auto it = std::begin(args), end = std::end(args);
			it != end; ++it) {
		std::cout << Variable::toString(*(*it)) << std::endl;
	}
}

void yield(std::vector<std::shared_ptr<Variable>>&, std::shared_ptr<Variable>) {
	std::cout << "YIELDING" << std::endl;
	terp->setYielded(true);
	lastYield = Time::getTime();
}

int main(int argc, char** argv) {
	lastYield = 0;

	std::ifstream inFile("test-statement.txt"); // TODO: not hardcode
	terp = std::make_unique<Interpreter>(inFile);

	terp->parseText();
	inFile.close();

	terp->addExternalFunc("add", add);
	terp->addExternalFunc("yield", yield);

	while (terp->isRunning()) {
		if (terp->hasScriptEvents()) {
			terp->processScript();
		}

		if (terp->isYielded() && (Time::getTime() - lastYield) >= 5) {
			terp->setYielded(false);
		}
	}

	if (terp->getVariable("z") != nullptr) {
		std::cout << "z = " << Variable::toString(*terp->getVariable("z")) << std::endl;
	}
	else {
		std::cout << "z is null" << std::endl;
	}

	return 0;
}
