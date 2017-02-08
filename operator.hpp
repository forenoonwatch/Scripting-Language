#pragma once

#include <string>

class Operator {
	public:
		enum OperatorType {
			UNARY,
			BINARY,
			OTHER
		};

		Operator(const std::string& label); // TODO: add action

		std::string getLabel() const;

		virtual ~Operator() = default;
	private:
		std::string label;
};

#include "operator.inl"
