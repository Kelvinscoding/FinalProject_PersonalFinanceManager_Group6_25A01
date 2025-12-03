#pragma once
#include <string>

class IncomeSource {
private:
	std::string sourceCode;
	std::string sourceName;

public:
	// Constructor
	IncomeSource(const std::string& code = "", const std::string& name = "")
		: sourceCode(code), sourceName(name) {}

	// Getters
	std::string getCode() const { return sourceCode; }
	std::string getName() const { return sourceName; }

	// Setters
	void setName(const std::string& newName) { sourceName = newName; }
};