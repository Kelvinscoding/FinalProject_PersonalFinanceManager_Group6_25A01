#pragma once
#include <string>

class ExpenseCategory {
private:
	std::string categoryCode;
	std::string categoryName;

public:
	// Constructor
	ExpenseCategory(const std::string& code = "", const std::string& name = "")
		: categoryCode(code), categoryName(name) {}

	// Getters
	std::string getCode() const { return categoryCode; }
	std::string getName() const { return categoryName; }

	// Setters
	void setName(const std::string& newName) { categoryName = newName; }
};