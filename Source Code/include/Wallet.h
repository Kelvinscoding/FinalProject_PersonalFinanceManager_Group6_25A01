#pragma once
#include <string>

class Wallet {
private:
	std::string walletCode;
	std::string walletName;
	double currentBalance;

public:
	// Constructor
	Wallet(const std::string& code = "", const std::string& name = "", double initialBalance = 0.0)
		: walletCode(code), walletName(name), currentBalance(initialBalance) {}

	// Getters
	std::string getCode() const { return walletCode; }
	std::string getName() const { return walletName; }
	double getBalance() const { return currentBalance; }

	// Setters
	void setName(const std::string& newName) { walletName = newName; }

    // Update Balance
	void updateBalance(double amount) { 
		currentBalance += amount; 
	}
};