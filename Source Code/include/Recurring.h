#pragma once
#include "Transaction.h"
#include "date.h"
#include <fstream>
#include <string>

class RecurringTask {
private:
    std::string id;
    flow type;
    long long amount;
    std::string description;
    std::string walletId;
    std::string categoryId;
    Date nextDue; //Constantly updating the next due date
    int originalDay; 

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

    RecurringTask() : id(""), amount(0), type(expense), originalDay(1) {
        nextDue = Date::getCurrentDate();
        originalDay = nextDue.getDay();
    }


    RecurringTask(std::string tId, flow tType, long long amt, std::string desc,
        std::string wId, std::string cId, Date start)
        : id(tId), type(tType), amount(amt), description(desc),
        walletId(wId), categoryId(cId), nextDue(start) {
        
        originalDay = start.getDay(); 
    }

    Date getNextDueDate() const { return nextDue; }
    std::string getId() const { return id; }
    std::string getWalletId() const { return walletId; }

    //Generate the Transaction
    Transaction generateTransaction(std::string newId) const {
        // Adds "(Recurring)" to description so user knows where it came from
        return Transaction(nextDue, type, amount, description + " (Recurring)",
            newId, walletId, categoryId);
    }

    // Advance month
    void advanceDueDate() {
        int d = originalDay;
        int m = nextDue.getMonth();
        int y = nextDue.getYear();

        m++;
        if (m > 12) {
            m = 1;
            y++;
        }

        //Handle cases in which the next month doesn't contain the recurring day
        int maxDays = Date::daysInMonth(m, y);
        if (d > maxDays) {
            d = maxDays;
        }

        nextDue = Date(d, m, y);
    }

    //Serialization
    void writeBinary(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&amount), sizeof(amount));
        
        int typeInt = static_cast<int>(type);
        out.write(reinterpret_cast<const char*>(&typeInt), sizeof(typeInt));
        
        int d = nextDue.getDay();
        int m = nextDue.getMonth();
        int y = nextDue.getYear();
        out.write(reinterpret_cast<const char*>(&d), sizeof(d));
        out.write(reinterpret_cast<const char*>(&m), sizeof(m));
        out.write(reinterpret_cast<const char*>(&y), sizeof(y));

        out.write(reinterpret_cast<const char*>(&originalDay), sizeof(originalDay));

        saveString(out, id);
        saveString(out, description);
        saveString(out, walletId);
        saveString(out, categoryId);
    }

    //Deserialization
    void readBinary(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&amount), sizeof(amount));

        int typeInt;
        in.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
        type = static_cast<flow>(typeInt);

        int d, m, y;
        in.read(reinterpret_cast<char*>(&d), sizeof(d));
        in.read(reinterpret_cast<char*>(&m), sizeof(m));
        in.read(reinterpret_cast<char*>(&y), sizeof(y));
        nextDue = Date(d, m, y);

        in.read(reinterpret_cast<char*>(&originalDay), sizeof(originalDay));

        loadString(in, id);
        loadString(in, description);
        loadString(in, walletId);
        loadString(in, categoryId);
    }
};