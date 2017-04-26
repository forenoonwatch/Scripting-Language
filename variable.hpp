#pragma once

#include "token.hpp"
#include "statement.hpp"
#include <string>

class Variable {
	public:
		enum VariableType {
			STRING,
			INT,
			FLOAT,
			BOOL,
			FUNCTION,
			NIL,
			OTHER
		};

		static Variable fromToken(const Token&);

		Variable() = default; // TODO: add constructors for various types possibly
		Variable(Statement* func);

		void cloneInto(Variable&) const;

		virtual ~Variable() = default;

		static void add(const Variable&, const Variable&, Variable&);
		static void sub(const Variable&, const Variable&, Variable&);
		static void mul(const Variable&, const Variable&, Variable&);
		static void div(const Variable&, const Variable&, Variable&);
		static void mod(const Variable&, const Variable&, Variable&);
		
		static void comp(const Variable&, const Variable&, Variable&);
		static void less(const Variable&, const Variable&, Variable&);
		static void lessEq(const Variable&, const Variable&, Variable&);

		static void unaryMinus(const Variable&, Variable&);

		static std::string toString(const Variable&);

		std::string stringValue;
		int intValue;
		double floatValue;
		bool boolValue;
		Statement* funcValue;

		VariableType type = VariableType::OTHER;
};
