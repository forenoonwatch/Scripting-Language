#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "interpreter.hpp"

void executeFile(const std::string&, ErrorLog::LogDepth);
void editScript(ErrorLog::LogDepth);
void runScriptFromStream(std::stringstream&, ErrorLog::LogDepth);

int main(int argc, char** argv) {
	ErrorLog::LogDepth depth = ErrorLog::LogDepth::WARNING;

	if (argc > 1) {
		std::string firstArg(argv[1]);

		if (firstArg.compare("-h") == 0 || firstArg.compare("--help") == 0) {
			std::cout << "Scripting Language" << std::endl;
			std::cout << "Type lits with no arguments to open" << std::endl;
			std::cout << "the line by line script editor." << std::endl;
			std::cout << "Or type lits [file name] to run a source file" << std::endl;

			std::cout << "\nFlags:" << std::endl;
			std::cout << "-f [none|error|warning|debug]\tlets you set the debug output level" << std::endl;
			std::cout << "-h (--help)\t\t\tshows the help output" << std::endl;
		}
		else if (firstArg.find("-f") != std::string::npos) {
			std::string level = firstArg.substr(2);
			std::transform(level.begin(), level.end(), level.begin(), ::tolower);

			if (level.compare("none") == 0) {
				depth = ErrorLog::LogDepth::NONE;
			}
			else if (level.compare("error") == 0) {
				depth = ErrorLog::LogDepth::ERROR;
			}
			else if (level.compare("warning") == 0) {
				depth = ErrorLog::LogDepth::WARNING;
			}
			else if (level.compare("debug") == 0) {
				depth = ErrorLog::LogDepth::DEBUG;
			}

			if (argc > 2) {
				executeFile(argv[2], depth);
			}
			else {
				editScript(depth);
			}
		}
		else {
			executeFile(firstArg, depth);
		}
	}
	else {
		editScript(depth);
	}

	return 0;
}

void executeFile(const std::string& fileName, ErrorLog::LogDepth depth) {
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
	terp.setLogDepth(depth);
	
	terp.parseText();
	inFile.close();

	while (terp.isRunning()) {
		if (terp.hasScriptEvents()) {
			terp.processScript();
		}
	}
}

void editScript(ErrorLog::LogDepth depth) {
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
	runScriptFromStream(ss, depth);
}

void runScriptFromStream(std::stringstream& stream, ErrorLog::LogDepth depth) {
	Interpreter terp(stream);
	
	terp.setLogDepth(depth);
	terp.parseText();

	while (terp.isRunning()) {
		if (terp.hasScriptEvents()) {
			terp.processScript();
		}
	}
}
