#pragma once
#include <string> 
#include <fstream>

class ExpenseCategory {
private:
    std::string id;
    std::string name;

    //Functions for serializing std::string
    void saveString(std::ofstream& out, const std::string& str) const {
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
    void setName(std::string newName) { name = newName; }

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

    //Serialization
    void writeBinary(std::ofstream& out) const {
        saveString(out, id);
        saveString(out, name);
    }

    //Deserialization
    void readBinary(std::ifstream& in) {
        loadString(in, id);
        loadString(in, name);
    }
};