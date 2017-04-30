#pragma once

#include "token.hpp"
#include "statement.hpp"
#include <string>
#include <vector>
#include <memory>

class Variable;
class Interpreter;

typedef void (*ExternalFunction)(std::vector<std::shared_ptr<Variable>>&,
	std::shared_ptr<Variable>, Interpreter&);

class Variable {
	public:
		enum VariableType {
			STRING,
			INT,
			FLOAT,
			BOOL,
			FUNCTION,
			EXTERNAL_FUNC,
			NIL,
			OTHER
		};

		static Variable fromToken(const Token&);

		Variable() = default; // TODO: add constructors for various types possibly
		Variable(Statement* func);
		Variable(ExternalFunction func);

		void cloneInto(Variable&) const;

		void operator()(std::vector<std::shared_ptr<Variable>>&,
			std::shared_ptr<Variable>, Interpreter&) const;

		double getAsFloat() const;
		int getAsInt() const;

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
		ExternalFunction efValue;

		VariableType type = VariableType::OTHER;
};
