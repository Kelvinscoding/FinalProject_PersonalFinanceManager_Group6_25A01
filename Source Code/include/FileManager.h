#pragma once
#include <fstream>
#include <iostream>
#include "DynamicArray.h"
#include "Transaction.h"
#include "Wallet.h"
#include "ExpenseCategory.h"
#include "IncomeSource.h"

class FileManager {
public:
    //SAVE EVERYTHING
    static bool saveData(const DynamicArray<Wallet>& wallets,
        const DynamicArray<IncomeSource>& sources,
        const DynamicArray<ExpenseCategory>& categories,
        const DynamicArray<Transaction>& transactions,
        const std::string& filename = "data.bin")
    {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            return false;
        }

        saveList(out, wallets);
        saveList(out, sources);
        saveList(out, categories);
        saveList(out, transactions);

        out.close();
        return true;
    }

    //LOAD EVERYTHING
    static bool loadData(DynamicArray<Wallet>& wallets,
        DynamicArray<IncomeSource>& sources,
        DynamicArray<ExpenseCategory>& categories,
        DynamicArray<Transaction>& transactions,
        const std::string& filename = "data.bin")
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in) return false; // File doesn't exist yet (First run)

        if (in.peek() == std::ifstream::traits_type::eof()) {
            return false; // Empty file, nothing to load
        }

        loadList(in, wallets);
        loadList(in, sources);
        loadList(in, categories);
        loadList(in, transactions);

        in.close();
        return true;
    }

private:
    template <typename T>

    //SAVE LIST
    static void saveList(std::ofstream& out, const DynamicArray<T>& list) {
        long long count = list.size();
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (long long i = 0; i < count; i++) {
            list[i].writeBinary(out);
        }
    }

    template <typename T>

    //LOAD LIST
    static void loadList(std::ifstream& in, DynamicArray<T>& list) {
        long long count = 0;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (long long i = 0; i < count; i++) {
            T item;
            item.readBinary(in);
            list.push_back(item);
        }
    }
};