#include <iostream>
#include <fstream>
#include <sstream>
#include "interpreter.hpp"

void executeFile(const std::string&);
void editScript();
void runScriptFromStream(std::stringstream& ss);

int main(int argc, char** argv) {
	if (argc > 1) {
		std::string firstArg(argv[1]);

		if (firstArg.compare("-h") == 0 || firstArg.compare("--help") == 0) {
			std::cout << "Scripting Language" << std::endl;
			std::cout << "Type lits with no arguments to open" << std::endl;
			std::cout << "the line by line script editor." << std::endl;
			std::cout << "Or type lits [file name] to run a source file" << std::endl;
		}
		else {
			executeFile(firstArg);
		}
	}
	else {
		// TODO: enter script editor
		editScript();
	}

	return 0;
}

void executeFile(const std::string& fileName) {
	std::ifstream inFile(fileName);

	inFile.seekg(0, inFile.end);
	int length = inFile.tellg();
	inFile.seekg(0, inFile.beg);

	if (length < 0) {
		std::cout << "Invalid file name: '" << fileName << "'" << std::endl;
		return;
	}

	std::cout << "Running file " << fileName << "..." << std::endl;

	Interpreter terp(inFile);
	
	terp.parseText();
	inFile.close();

	while (terp.isRunning()) {
		if (terp.hasScriptEvents()) {
			terp.processScript();
		}
	}
}

void editScript() {
	std::cout << "LITS script editor, type 'exit' to quit and run" << std::endl;

	std::string line;
	bool cont;

	std::stringstream ss;

	do {
		std::getline(std::cin, line);
		cont = line.compare("exit") != 0;

		if (cont) {
			ss << line << "\n";
		}
	}
	while (cont);

	std::cout << "Running script..." << std::endl;

	ss.flush();
	runScriptFromStream(ss);
}

void runScriptFromStream(std::stringstream& stream) {
	Interpreter terp(stream);
	terp.parseText();

	while (terp.isRunning()) {
		if (terp.hasScriptEvents()) {
			terp.processScript();
		}
	}
}
