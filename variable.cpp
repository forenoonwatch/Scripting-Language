#include "variable.hpp"

void Variable::add(const Variable& a, const Variable& b, Variable& out) {
	if (a.type == VariableType::STRING) {
		out.type = VariableType::STRING;

		if (b.type == VariableType::STRING) {
			out.stringValue = a.stringValue + b.stringValue;
		}
		else if (b.type == VariableType::INT) {
			out.stringValue = a.stringValue + b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.stringValue = a.stringValue + b.floatValue;
		}
		else if (b.type == Variable::BOOL) {
			out.stringVBalue = a.stringValue + b.boolValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::INT) {
		out.type = VariableType::INT;

		if (b.type == VariableType::STRING) {
			out.stringValue = a.intValue + b.stringValue;
		}
		else if (b.type == VariableType::INT) {
			out.stringValue = a.intValue + b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.stringValue = a.intValue + b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
}
