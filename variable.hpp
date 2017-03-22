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

		static void add(const Variable&, const Variable&, Variable&);
		static void sub(const Variable&, const Variable&, Variable&);
		static void mul(const Variable&, const Variable&, Variable&);
		static void div(const Variable&, const Variable&, Variable&);
		static void mod(const Variable&, const Variable&, Variable&);
		
		static void comp(const Variable&, const Variable&, Variable&);
		static void less(const Variable&, const Variable&, Variable&);
		static void lessEq(const Variable&, const Variable&, Variable&);

		std::string stringValue;
		int intValue;
		double floatValue;
		bool boolValue;

		VariableType type = VariableType::OTHER;
};
