#pragma once
#include <string>
#include "date.h"

enum flow { //Define for both income management and expense management
	income,
	expense
};

class Transaction {
private: 
	Date date;
	flow transactionType;
	long long amount;
	std::string description;
	std::string id;
	std::string walletId;
	std::string categoryId; //Stands for SourceId (income) or CategoryId(outcome)

public:
	Transaction() : id(""), transactionType(expense), amount(0), description(""), walletId(""), categoryId("") {
		date = Date::getCurrentDate();
	}

	Transaction(Date d, flow tType, long long amt, std::string des, std::string tId, std::string wId, std::string cId)
		: date(d), transactionType(tType), amount(amt),
		description(des), id(tId), walletId(wId), categoryId(cId) {
	}
	
	Date getDate() const { return date; }

	flow getType() const { return transactionType; }

	long long getAmount() const { return amount; }

	std::string getDescription() const { return description; }

	std::string getId() const { return id; }

	std::string getWalletId() const { return walletId; }

	std::string getCategoryId() const { return categoryId; }

	bool isIncome() const { return transactionType == income; }

	bool isExpense() const { return transactionType == expense; }
};