#pragma once
#include <string>

class IncomeSource {
private:
    std::string id;
    std::string name;
public:
    IncomeSource() : id(""), name("") {}
    IncomeSource(std::string newId, std::string newName) {
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