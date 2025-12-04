#pragma once
#include <string>

class Wallet {
private:
    std::string id;
    std::string name;
    long long balance;
public:
    Wallet() : id(""), name(""), balance(0) {}
    Wallet(std::string newId, std::string newName, long long start = 0) {
        id = newId;
        name = newName;
        balance = start;
    }
    std::string getId() const { 
        return id; 
    }
    std::string getName() const { 
        return name; 
    }
    long long getBalance() const { 
        return balance; 
    }
    void setBalance(long long newBalance) {
        balance = newBalance;
    }
};