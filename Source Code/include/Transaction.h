#pragma once
#include <string>
#include <fstream>
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

    //Functions for serializing std::string 
    void saveString(std::ofstream& out, const std::string& str) const{
        size_t len = str.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(str.data(), len);
    }

    void loadString(std::ifstream& in, std::string& str) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        str.resize(len);
        in.read(&str[0], len);
    }

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

    //Serialization
    void writeBinary(std::ofstream& out) const{
        out.write(reinterpret_cast<const char*>(&amount), sizeof(amount));

        int typeInt = static_cast<int>(transactionType);
        out.write(reinterpret_cast<const char*>(&typeInt), sizeof(typeInt));

        int d = date.getDay();
        int m = date.getMonth();
        int y = date.getYear();
        out.write(reinterpret_cast<const char*>(&d), sizeof(d));
        out.write(reinterpret_cast<const char*>(&m), sizeof(m));
        out.write(reinterpret_cast<const char*>(&y), sizeof(y));

        saveString(out, description);
        saveString(out, id);
        saveString(out, walletId);
        saveString(out, categoryId);
    }

    //Deserialization
    void readBinary(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&amount), sizeof(amount));

        int typeInt;
        in.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
        transactionType = static_cast<flow>(typeInt);

        int d, m, y;
        in.read(reinterpret_cast<char*>(&d), sizeof(d));
        in.read(reinterpret_cast<char*>(&m), sizeof(m));
        in.read(reinterpret_cast<char*>(&y), sizeof(y));

        date = Date(d, m, y);

        loadString(in, description);
        loadString(in, id);
        loadString(in, walletId);
        loadString(in, categoryId);
    }
};