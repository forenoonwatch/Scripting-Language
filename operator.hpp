#pragma once

#include <string>

class Operator {
	public:
		Operator(const std::string& label); // TODO: add action

		std::string getLabel() const;

		virtual ~Operator() = default;
	private:
		std::string label;
};

#include "operator.inl"
