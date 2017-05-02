#include "default-lib.hpp"

#include <iostream>
#include <sstream>
#include <chrono>

using namespace std::chrono;

namespace Time {
	high_resolution_clock::time_point start = high_resolution_clock::now();

	double getTime() {
		return duration_cast<duration<double>>(high_resolution_clock::now()
			- start).count();
	}
};

DefaultLibrary::DefaultLibrary(Interpreter& interpreter): interpreter(interpreter) {}

void DefaultLibrary::loadExternalFunctions() {
	// IO functions
	interpreter.addExternalFunc("print", print);
	interpreter.addExternalFunc("get_input", getInput);

	// Thread functions
	interpreter.addExternalFunc("yield", yield);
	interpreter.addExternalFunc("sleep", sleep);

	// General purpose functions
	interpreter.addExternalFunc("to_string", toString);
	interpreter.addExternalFunc("to_number", toNumber);
}

void DefaultLibrary::update() {
	if (interpreter.isYielded() && Time::getTime() - lastYield >= yieldTime) {
		interpreter.setYielded(false);
	}
}

// default function definitions

void DefaultLibrary::print(std::vector<std::shared_ptr<Variable>>& args,
		std::shared_ptr<Variable>, Interpreter& interpreter) {
	std::stringstream ss;

	for (auto it = std::begin(args), end = std::end(args); it != end; ++it) {
		ss << Variable::toString(*(*it));
	}

	std::cout << ss.str() << std::endl;
}

void DefaultLibrary::getInput(std::vector<std::shared_ptr<Variable>>&,
		std::shared_ptr<Variable> out, Interpreter&) {
	out->type = Variable::VariableType::STRING;
	std::getline(std::cin, out->stringValue);
}

void DefaultLibrary::toString(std::vector<std::shared_ptr<Variable>>& args,
		std::shared_ptr<Variable> out, Interpreter&) {
	out->type = Variable::VariableType::STRING;
	out->stringValue = Variable::toString(*args[0]);
}

void DefaultLibrary::toNumber(std::vector<std::shared_ptr<Variable>>& args,
		std::shared_ptr<Variable> out, Interpreter&) {
	if (args[0]->type == Variable::VariableType::INT) {
		out->type = Variable::VariableType::INT;
		out->intValue = args[0]->intValue;
	}
	else {
		out->type = Variable::VariableType::FLOAT;
		out->floatValue = args[0]->getAsFloat();
	}
}

void DefaultLibrary::yield(std::vector<std::shared_ptr<Variable>>&,
		std::shared_ptr<Variable>, Interpreter& interpreter) {
	interpreter.setYielded(true);

	interpreter.defaultLib->lastYield = Time::getTime();
	interpreter.defaultLib->yieldTime = 0;
}

void DefaultLibrary::sleep(std::vector<std::shared_ptr<Variable>>& args,
		std::shared_ptr<Variable>, Interpreter& interpreter) {
	interpreter.setYielded(true);

	interpreter.defaultLib->lastYield = Time::getTime();
	interpreter.defaultLib->yieldTime = args[0]->getAsFloat();
}
