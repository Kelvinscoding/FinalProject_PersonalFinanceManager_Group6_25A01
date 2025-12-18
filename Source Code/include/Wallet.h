#pragma once
#include <string>
#include <fstream>

class Wallet {
private:
    std::string id;
    std::string name;
    long long balance;

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

    //Serialization
    void writeBinary(std::ofstream& out) const {
        saveString(out, id);
        saveString(out, name);
        out.write(reinterpret_cast<const char*>(&balance), sizeof(balance));
    }

    //Deserialization
    void readBinary(std::ifstream& in) {
        loadString(in, id);
        loadString(in, name);
        in.read(reinterpret_cast<char*>(&balance), sizeof(balance));
    }
};