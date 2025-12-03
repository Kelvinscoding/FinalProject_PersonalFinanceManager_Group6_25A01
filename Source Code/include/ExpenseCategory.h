#pragma once
#include <string> 

class ExpenseCategory {
private:
    std::string id;
    std::string name;
public:
    ExpenseCategory() : id(""), name("") {}
    ExpenseCategory(std::string newId, std::string newName) {
        id = newId;
        name = newName;
    }
    std::string getId() const {
        return id;
    }
    std::string getName() const {
        return name;
    }
};