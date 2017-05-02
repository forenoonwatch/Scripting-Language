#pragma once

#include "interpreter.hpp"

class DefaultLibrary {
	public:
		DefaultLibrary(Interpreter&);
		
		void loadExternalFunctions();
		void update();
	private:
		Interpreter& interpreter;

		double lastYield;
		double yieldTime;

		// all internal library functions ////////////////////////////////////////
		
		/**
		 * print(...) -> void
		 *
		 * Takes in any number of arguments and prints them out concatenated together
		 * followed by an end-line
		 */
		static void print(std::vector<std::shared_ptr<Variable>>&,
			std::shared_ptr<Variable>, Interpreter&);

		/**
		 * get_input(void) -> String
		 *
		 * Pauses for input and returns the string received
		 */
		static void getInput(std::vector<std::shared_ptr<Variable>>&,
			std::shared_ptr<Variable>, Interpreter&);

		/**
		 * to_string(var) -> String
		 *
		 * Gets the string representation of a variable
		 */
		static void toString(std::vector<std::shared_ptr<Variable>>&,
			std::shared_ptr<Variable>, Interpreter&);
		
		/**
		 * to_number(var) -> float
		 *
		 * Gets the numerical representation of a variable
		 */
		static void toNumber(std::vector<std::shared_ptr<Variable>>&,
			std::shared_ptr<Variable>, Interpreter&);

		/**
		 * yield(void) -> void
		 *
		 * Yields the script interpreter for one frame
		 */
		static void yield(std::vector<std::shared_ptr<Variable>>&,
			std::shared_ptr<Variable>, Interpreter&);

		/**
		 * sleep(double sleepTime) -> void
		 *
		 * @param sleepTime how long to sleep in milliseconds
		 *
		 * pauses the script interpreter for [sleepTime] seconds
		 */
		static void sleep(std::vector<std::shared_ptr<Variable>>&,
			std::shared_ptr<Variable>, Interpreter&);
};
