#include "operator.hpp"

Operator::Operator(const std::string& label, ArgumentType argType, OperatorType opType)
: label(label), argumentType(argType), operatorType(opType) {}
