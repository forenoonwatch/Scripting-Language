#pragma once

#include <string>

class ScriptError {
	public:
		virtual std::string getError() const = 0;
};

class SyntaxError : public ScriptError {
	public:
		SyntaxError(const std::string& expected, const std::string& got);

		std::string getError() const override;

		virtual ~SyntaxError() = default;
	private:
		std::string expected;
		std::string got;
};

class RuntimeError : public ScriptError {
	public:
		std::string getError() const override;
};
