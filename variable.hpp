#pragma once

#include <string>

class Variable {
	public:
		enum VariableType {
			STRING,
			INT,
			FLOAT,
			BOOL,
			OTHER // TODO: add support for being function references
		};

		Variable() = default; // TODO: add constructors for various types possibly

		virtual ~Variable() = default;

		std::string stringValue;
		int intValue;
		double floatValue;
		bool boolValue;

		VariableType type = VariableType::OTHER;
};
