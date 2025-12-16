#include <iostream>
#include <string>
#include <iomanip>


#include "../include/DynamicArray.h"
#include "../include/date.h"
#include "../include/Wallet.h"
#include "../include/IncomeSource.h"
#include "../include/ExpenseCategory.h"
#include "../include/Transaction.h"
#include "../include/Recurring.h"
#include "../include/FileManager.h"

using namespace std;

// 1. GLOBAL DATA CONTAINERS
DynamicArray<Wallet> wallets;
DynamicArray<IncomeSource> incomeSources;
DynamicArray<ExpenseCategory> expenseCategories;
DynamicArray<Transaction> transactions;
DynamicArray<RecurringTask> recurringTasks;


// 2. HELPER FUNCTIONS
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause() {
    cout << "\nPress Enter to continue";
    cin.ignore(1000, '\n');
    cin.get();
}

// Checks if any recurring tasks are due today
void processRecurringTasks() {
    Date today = Date::getCurrentDate();
    int addedCount = 0;

    for (long long i = 0; i < recurringTasks.size(); i++) {
        // Check if the task is due (or overdue from missed days)
        while (recurringTasks[i].getNextDueDate() < today || recurringTasks[i].getNextDueDate() == today) {
            
            // Generate the transaction from the recurring task
            string newId = transactions.generateNewId("TRX");
            Transaction t = recurringTasks[i].generateTransaction(newId);
            transactions.push_back(t);

            // Automatically update the wallet balance
            for (long long w = 0; w < wallets.size(); w++) {
                if (wallets[w].getId() == t.getWalletId()) {
                    long long amt = t.getAmount();
                    
                    if (t.isExpense()) {
                        wallets[w].setBalance(wallets[w].getBalance() - amt);
                    } else {
                        wallets[w].setBalance(wallets[w].getBalance() + amt);
                    }
                    break;
                }
            }

            recurringTasks[i].advanceDueDate();
            addedCount++;
        }
    }

    if (addedCount > 0) {
        cout << " [SYSTEM]: Automatically processed " << addedCount << " recurring transaction(s)." << endl;
        pause();
    }
}



void viewDashboard() {
    clearScreen();
    cout << "------------------------------------------" << endl;
    cout << "               DASHBOARD                  " << endl;
    cout << "------------------------------------------" << endl;
    
    // Show Wallets
    long long totalNetWorth = 0;
    if (wallets.size() == 0) {
        cout << "No wallets found. Please add one in Settings." << endl;
    } else {
        cout << left << setw(20) << "Wallet Name" << right << setw(15) << "Balance (VND)" << endl;
        cout << string(40, '-') << endl;
        for (long long i = 0; i < wallets.size(); i++) {
            cout << left << setw(20) << wallets[i].getName() 
                 << right << setw(15) << wallets[i].getBalance() << endl;
            totalNetWorth += wallets[i].getBalance();
        }
        cout << string(40, '-') << endl;
        cout << left << setw(20) << "TOTAL NET WORTH:" << right << setw(15) << totalNetWorth << endl;
    }
    
    // Show Recent Transactions (Last 5)
    cout << "\n---Recent Activity---" << endl;
    if (transactions.size() == 0) {
        cout << " (No transactions yet)" << endl;
    } else {
        long long start = 0;
        if (transactions.size() > 5) start = transactions.size() - 5;

        for (long long i = start; i < transactions.size(); i++) {
            Transaction& t = transactions[i];
            cout << t.getDate().getDay() << "/" << t.getDate().getMonth() << " | "
                 << left << setw(20) << t.getDescription().substr(0, 19);
            
            if (t.isIncome()) {
                cout << " | + " << t.getAmount();
            } else {
                cout << " | - " << t.getAmount();
            }
            cout << endl;
        }
    }
    pause();
}

// Statistics & Reports Menu
void statisticsMenu() {
    while (true) {
        clearScreen();
        cout << "---STATISTICS & REPORTS---" << endl;
        cout << "1. Monthly Overview (Income vs Expense)" << endl;
        cout << "2. View All Transactions" << endl;
        cout << "3. Back to Main Menu" << endl;
        cout << "Choice: ";
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); cin.ignore(1000, '\n'); continue;
        }

        if (choice == 3) break;

        if (choice == 1) {
            int m, y;
            cout << "\nEnter Month (1-12): "; cin >> m;
            cout << "Enter Year: "; cin >> y;

            long long totalInc = 0;
            long long totalExp = 0;

            cout << "\n---Report for " << m << "/" << y << "---" << endl;
            bool found = false;
            for (long long i = 0; i < transactions.size(); i++) {
                Date d = transactions[i].getDate();
                if (d.getMonth() == m && d.getYear() == y) {
                    found = true;
                    if (transactions[i].isIncome()) totalInc += transactions[i].getAmount();
                    else totalExp += transactions[i].getAmount();
                }
            }

            if (!found) {
                cout << "No transactions found for this period" << endl;
            } else {
                cout << "Total Income:  + " << totalInc << " VND" << endl;
                cout << "Total Expense: - " << totalExp << " VND" << endl;

                long long net = totalInc - totalExp;
                cout << "Net Savings:   " << (net >= 0 ? "+ " : "") << net << " VND" << endl;
            }
            pause();
        }
        else if (choice == 2) {
            clearScreen();
            cout << "---ALL TRANSACTIONS---" << endl;
            cout << left << setw(12) << "Date" << setw(10) << "Type" << setw(25) << "Description" << right << setw(15) << "Amount" << endl;
            cout << string(65, '-') << endl;

            for (long long i = 0; i < transactions.size(); i++) {
                Transaction& t = transactions[i];
                string typeStr = t.isIncome() ? "INCOME" : "EXPENSE";

                cout << t.getDate().getDay() << "/" << t.getDate().getMonth() << "/" << t.getDate().getYear() << "   "
                    << left << setw(10) << typeStr
                    << setw(25) << t.getDescription().substr(0, 24)
                    << right << setw(15) << t.getAmount() << endl;
            }
            pause();
        }
    }
}

// Settings Menu Functions

void addWallet() {
    string name;
    cout << "Enter Wallet Name:";
    cin.ignore();
    getline(cin, name);
    long long bal;
    cout << "Enter Initial Balance: ";
    cin >> bal;

    string id = wallets.generateNewId("WAL");
    wallets.push_back(Wallet(id, name, bal));
    cout << "Wallet added successfully!" << endl;
}

void addCategory() {
    string name;
    cout << "Enter Expense Category (e.g., Food, Rent): ";
    cin.ignore();
    getline(cin, name);
    string id = expenseCategories.generateNewId("CAT");
    expenseCategories.push_back(ExpenseCategory(id, name));
    cout << "Category added successfully!" << endl;
}

void addSource() {
    string name;
    cout << "Enter Income Source (e.g., Salary, Gift): ";
    cin.ignore();
    getline(cin, name);
    string id = incomeSources.generateNewId("SRC");
    incomeSources.push_back(IncomeSource(id, name));
    cout << "Source added successfully!" << endl;
}

// Add Transaction Menu
void addTransactionMenu() {
    clearScreen();

    if (wallets.size() == 0) {
        cout << "Error: You must add a Wallet first (Go to Settings)." << endl;
        pause();
        return;
    }
    if (incomeSources.size() == 0 && expenseCategories.size() == 0) {
        cout << "Error: You must add Categories or Sources first (Go to Settings)." << endl;
        pause();
        return;
    }

    cout << "---ADD NEW TRANSACTION---" << endl;
    cout << "1. Income" << endl;
    cout << "2. Expense" << endl;
    cout << "Select Type: ";
    int typeChoice;
    cin >> typeChoice;

    if (cin.fail() || (typeChoice != 1 && typeChoice != 2)) {
        cin.clear(); cin.ignore(1000, '\n'); return;
    }

    flow type = (typeChoice == 1) ? income : expense;

    cout << "\n--- Select Wallet ---" << endl;
    for (long long i = 0; i < wallets.size(); i++) {
        cout << i + 1 << ". " << wallets[i].getName() << " (Bal: " << wallets[i].getBalance() << ")" << endl;
    }
    int wIdx;
    cout << "Choice: ";
    cin >> wIdx;

    if (wIdx < 1 || wIdx > wallets.size()) return;
    string wId = wallets[wIdx - 1].getId();

    string catSourceId;
    cout << "\n---Select Category/Source---" << endl;
    if (type == income) {
        if (incomeSources.size() == 0) { cout << "No Income Sources defined" << endl; pause(); return; }
        for (long long i = 0; i < incomeSources.size(); i++) {
            cout << i + 1 << ". " << incomeSources[i].getName() << endl;
        }
        int sIdx;
        cout << "Choice: "; cin >> sIdx;
        if (sIdx < 1 || sIdx > incomeSources.size()) return;
        catSourceId = incomeSources[sIdx - 1].getId();
    } else {
        if (expenseCategories.size() == 0) { cout << "No Expense Categories defined" << endl; pause(); return; }
        for (long long i = 0; i < expenseCategories.size(); i++) {
            cout << i + 1 << ". " << expenseCategories[i].getName() << endl;
        }
        int cIdx;
        cout << "Choice: "; cin >> cIdx;
        if (cIdx < 1 || cIdx > expenseCategories.size()) return;
        catSourceId = expenseCategories[cIdx - 1].getId();
    }

    long long amount;
    cout << "\nEnter Amount: ";
    cin >> amount;
    if (cin.fail()) {
        cin.clear(); cin.ignore(1000, '\n');
        cout << "Invalid amount" << endl;
        return;
    }
    
    string desc;
    cout << "Enter Description: ";
    cin.ignore();
    getline(cin, desc);


    string tId = transactions.generateNewId("TRX");
    Date now = Date::getCurrentDate();

    Transaction t(now, type, amount, desc, tId, wId, catSourceId);
    transactions.push_back(t);

    long long currentBal = wallets[wIdx - 1].getBalance();
    if (type == income) currentBal += amount;
    else currentBal -= amount;
    wallets[wIdx - 1].setBalance(currentBal);

    cout << "\nTransaction Added Successfully!" << endl;
    pause();
}

void settingsMenu() {
    while (true) {
        clearScreen();
        cout << "=== MASTER DATA SETTINGS ===" << endl;
        cout << "1. Add New Wallet" << endl;
        cout << "2. Add Expense Category" << endl;
        cout << "3. Add Income Source" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << "Choice: ";
        int c; cin >> c;

        if (c == 4) break;

        switch (c) {
        case 1: addWallet(); break;
        case 2: addCategory(); break;
        case 3: addSource(); break;
        }
        if (c >= 1 && c <= 3) pause();
    }
}


int main() {

    FileManager::loadData(wallets, incomeSources, expenseCategories, transactions, recurringTasks);

    processRecurringTasks();

    while (true) {
        clearScreen();
        cout << "------------------------------------------" << endl;
        cout << "       PERSONAL FINANCE MANAGER           " << endl;
        cout << "------------------------------------------" << endl;
        cout << "1. Dashboard (Overview)" << endl;
        cout << "2. Add Transaction" << endl;
        cout << "3. Statistics & Reports" << endl;
        cout << "4. Settings (Wallets/Categories)" << endl;
        cout << "0. Save & Exit" << endl;
        cout << "------------------------------------------" << endl;
        cout << "Enter Choice: ";
        
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
        case 1:
            viewDashboard();
            break;
        case 2:
            addTransactionMenu();
            break;
        case 3:
            statisticsMenu();
            break;
        case 4:
            settingsMenu();
            break;
        case 0:
            cout << "\nSaving data" << endl;
            FileManager::saveData(wallets, incomeSources, expenseCategories, transactions, recurringTasks);
            cout << "Data saved. Goodbye!" << endl;
            return 0;
        default:
            cout << "Invalid choice." << endl;
            pause();
        }
    }
}