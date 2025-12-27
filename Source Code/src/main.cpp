//MAIN FILE FOR THE FINANCE MANAGER APP PROJECT
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
    int errorCount = 0;

    for (long long i = 0; i < recurringTasks.size(); i++) {
        // Check if the task is expired
        if (recurringTasks[i].isExpired()) {
            continue;
        }
        // Check if the task is due
        while (recurringTasks[i].getNextDueDate() < today || recurringTasks[i].getNextDueDate() == today) {
            if (recurringTasks[i].isExpired()) {
                break;
            }
            // Check if the wallet still exists before creating transaction
            bool walletFound = false;
            int walletIndex = -1;
            
            // Temp transaction to check
            string tempId = transactions.generateNewId("TRX");
            Transaction t = recurringTasks[i].generateTransaction(tempId);
            
            for (long long w = 0; w < wallets.size(); w++) {
                if (wallets[w].getId() == t.getWalletId()) {
                    walletFound = true;
                    walletIndex = w;
                    break;
                }
            }

            if (walletFound) {
                transactions.push_back(t);

                long long amt = t.getAmount();
                if (t.isExpense()) {
                    wallets[walletIndex].setBalance(wallets[walletIndex].getBalance() - amt);
                } else {
                    wallets[walletIndex].setBalance(wallets[walletIndex].getBalance() + amt);
                }
                addedCount++;
            } else {
                errorCount++;
            }
            recurringTasks[i].advanceDueDate();
        }
    }

    if (addedCount > 0) {
        cout << " [SYSTEM]: Automatically processed " << addedCount << " recurring transaction(s)" << endl;
        if (errorCount > 0) {
            cout << " [SYSTEM]: Skipped " << errorCount << " task(s) because the target wallet was missing" << endl;
        }
        pause();
    }
}

string getWalletName(string id) {
    for (long long i = 0; i < wallets.size(); i++)
        if (wallets[i].getId() == id) return wallets[i].getName();
    return "Unknown";
}

string getCategoryName(string id, bool isIncome) {
    if (isIncome) {
        for (long long i = 0; i < incomeSources.size(); i++)
            if (incomeSources[i].getId() == id) return incomeSources[i].getName();
    }
    else {
        for (long long i = 0; i < expenseCategories.size(); i++)
            if (expenseCategories[i].getId() == id) return expenseCategories[i].getName();
    }
    return "Unknown";
}

void viewDashboard() {
    clearScreen();
    cout << "------------------------------------------" << endl;
    cout << "               DASHBOARD                  " << endl;
    cout << "------------------------------------------" << endl;
    
    // Show wallets
    long long totalNetWorth = 0;
    if (wallets.size() == 0) {
        cout << "No wallets found. Please add one in Settings." << endl;
    } else {
        cout << left << setw(20) << "Wallet name" << right << setw(15) << "Balance (VND)" << endl;
        cout << string(40, '-') << endl;
        for (long long i = 0; i < wallets.size(); i++) {
            cout << left << setw(20) << wallets[i].getName() 
                 << right << setw(15) << wallets[i].getBalance() << endl;
            totalNetWorth += wallets[i].getBalance();
        }
        cout << string(40, '-') << endl;
        cout << left << setw(20) << "TOTAL NET WORTH:" << right << setw(15) << totalNetWorth << endl;
    }
    
    // Show recent transactions (last 5)
    cout << "\n---Recent Activity---" << endl;
    if (transactions.size() == 0) {
        cout << " (No transactions yet)" << endl;
    } else {
        long long start = 0;
        if (transactions.size() > 5) start = transactions.size() - 5;

        for (long long i = start; i < transactions.size(); i++) {
            Transaction& t = transactions[i];
            string wName = getWalletName(t.getWalletId());
            string catName = getCategoryName(t.getCategoryId(), t.isIncome());

            cout << t.getDate().getDay() << "/" << t.getDate().getMonth() << " | "
                 << left << setw(15) << wName.substr(0,14) 
                 << setw(15) << catName.substr(0, 14)
                 << setw(20) << t.getDescription().substr(0, 19);
            
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

// STATISTICS & REPORTS MENU

//Input a date from user
Date inputDate(string prompt) {
    cout << prompt << endl;
    int d, m, y;
    while (true) {
        cout << "Day: "; cin >> d;
        cout << "Month: "; cin >> m;
        cout << "Year: "; cin >> y;
        if (cin.fail()) {
            cout << "Invalid format. Please enter numbers for day, then month and year." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        if (m < 1 || m > 12 || d < 1 || d > 31) {
            cout << "Invalid date. Try again.\n";
            continue;
        }
        return Date(d, m, y);
    }
}

// 1. Time-based statistics
void statsTimeRange() {
    clearScreen();
    cout << "---TIME RANGE REPORT---" << endl;
    Date start = inputDate("Enter Start Date:");
    Date end = inputDate("Enter End Date:");

    long long totalInc = 0;
    long long totalExp = 0;
    bool found = false;

    cout << "\nAnalyzing transactions from "
        << start.getDay() << "/" << start.getMonth() << "/" << start.getYear()
        << " to "
        << end.getDay() << "/" << end.getMonth() << "/" << end.getYear() << "..." << endl;

    for (long long i = 0; i < transactions.size(); i++) {
        Date d = transactions[i].getDate();
        if (!(d < start) && !(d > end)) {
            found = true;
            if (transactions[i].isIncome()) totalInc += transactions[i].getAmount();
            else totalExp += transactions[i].getAmount();
        }
    }

    if (!found) {
        cout << "\nNo transactions found in this range." << endl;
    }
    else {
        cout << "\n---RESULTS---" << endl;
        cout << "Total Income:  + " << totalInc << endl;
        cout << "Total Expense: - " << totalExp << endl;
        long long net = totalInc - totalExp;
        cout << "Net Balance:   " << (net >= 0 ? "+ " : "") << net << endl;
    }
    pause();
}

// 2. Time & wallet-based statistics
void statsWalletTime() {
    clearScreen();
    if (wallets.size() == 0) { cout << "No wallets." << endl; pause(); return; }

    cout << "---WALLET TIME REPORT---" << endl;
    for (long long i = 0; i < wallets.size(); i++) {
        cout << i + 1 << ". " << wallets[i].getName() << endl;
    }
    cout << "Select Wallet: ";
    int wIdx; cin >> wIdx;
    if (cin.fail() || wIdx < 1 || wIdx > wallets.size()) {
        cin.clear(); cin.ignore(1000, '\n'); return;
    }
    string targetWId = wallets[wIdx - 1].getId();

    Date start = inputDate("\nEnter Start Date:");
    Date end = inputDate("Enter End Date:");

    long long totalInc = 0;
    long long totalExp = 0;
    bool found = false;

    for (long long i = 0; i < transactions.size(); i++) {
        Transaction& t = transactions[i];

        if (t.getWalletId() == targetWId) {
            Date d = t.getDate();
            if (!(d < start) && !(d > end)) {
                found = true;
                if (t.isIncome()) totalInc += t.getAmount();
                else totalExp += t.getAmount();
            }
        }
    }

    cout << "\n---RESULTS FOR " << wallets[wIdx - 1].getName() << "---" << endl;
    cout << "Total Income:  + " << totalInc << endl;
    cout << "Total Expense: - " << totalExp << endl;
    cout << "Net Change:    " << (totalInc - totalExp) << endl;
    pause();
}

// 3. Annual income/expense overview
void statsAnnualOverview() {
    clearScreen();
    cout << "---ANNUAL OVERVIEW---" << endl;

    DynamicArray<int> selectedYears;
    while (true) {
        cout << "Enter a year to add (e.g. 2024), or 0 to finish selection: ";
        int y; cin >> y;
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        if (y == 0) break;
        selectedYears.push_back(y);
    }

    if (selectedYears.size() == 0) return;

    long long totalInc = 0;
    long long totalExp = 0;

    for (long long i = 0; i < transactions.size(); i++) {
        int tYear = transactions[i].getDate().getYear();

        // Check if transaction year is in selected list
        bool yearMatch = false;
        for (long long k = 0; k < selectedYears.size(); k++) {
            if (selectedYears[k] == tYear) {
                yearMatch = true; break;
            }
        }

        if (yearMatch) {
            if (transactions[i].isIncome()) totalInc += transactions[i].getAmount();
            else totalExp += transactions[i].getAmount();
        }
    }

    cout << "\n---TOTALS FOR SELECTED YEAR(S)---" << endl;
    cout << "Total Income:  + " << totalInc << endl;
    cout << "Total Expense: - " << totalExp << endl;
    cout << "Net Balance:   " << (totalInc - totalExp) << endl;
    pause();
}

// 4. Annual income/expense breakdown by source/category
void statsAnnualBreakdown(bool isIncomeReport) {
    clearScreen();
    if (isIncomeReport) cout << "---INCOME SOURCE BREAKDOWN (Year-based)---" << endl;
    else cout << "---EXPENSE CATEGORY BREAKDOWN (Year-based)---" << endl;

    DynamicArray<int> selectedYears;
    while (true) {
        cout << "Enter a year to add (0 to finish): ";
        int y; cin >> y;
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        if (y == 0) break;
        selectedYears.push_back(y);
    }
    if (selectedYears.size() == 0) return;

    cout << left << setw(25) << (isIncomeReport ? "Source Name" : "Category Name")
        << right << setw(15) << "Total Amount" << endl;
    cout << string(40, '-') << endl;

    // Loop through sources/categories
    long long masterSize = isIncomeReport ? incomeSources.size() : expenseCategories.size();

    for (long long m = 0; m < masterSize; m++) {
        string id;
        string name;
        if (isIncomeReport) {
            id = incomeSources[m].getId();
            name = incomeSources[m].getName();
        }
        else {
            id = expenseCategories[m].getId();
            name = expenseCategories[m].getName();
        }

        long long currentTotal = 0;

        // Sum matching transactions
        for (long long t = 0; t < transactions.size(); t++) {
            Transaction& trx = transactions[t];

            // Must match type (income/expense)
            if (isIncomeReport && !trx.isIncome()) continue;
            if (!isIncomeReport && trx.isIncome()) continue;

            // Must match ID
            if (trx.getCategoryId() == id) {
                // Must match Year
                int tYear = trx.getDate().getYear();
                for (long long k = 0; k < selectedYears.size(); k++) {
                    if (selectedYears[k] == tYear) {
                        currentTotal += trx.getAmount();
                        break;
                    }
                }
            }
        }

        if (currentTotal > 0) {
            cout << left << setw(25) << name
                << right << setw(15) << currentTotal << endl;
        }
    }
    pause();
}

void statisticsMenu() {
    while (true) {
        clearScreen();
        cout << "---STATISTICS & REPORTS---" << endl;
        cout << "1. Monthly overview (Income vs Expense)" << endl;
        cout << "2. Time-based Report (Range)" << endl;
        cout << "3. Wallet-based Report (Range)" << endl;
        cout << "4. Annual Overview" << endl;
        cout << "5. Annual Income Breakdown" << endl;
        cout << "6. Annual Expense Breakdown" << endl;
        cout << "7. View All Transactions" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Choice: ";
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); cin.ignore(1000, '\n'); continue;
        }

        if (choice == 0) break;

        switch (choice) {
            case 1: {
                int m, y;
                while (true) {
                    cout << "\nEnter month (1-12): "; cin >> m;
                    cout << "Enter year: "; cin >> y;
                    if (cin.fail()) {
                        cout << "Invalid input. Please enter numbers." << endl;
                        cin.clear();
                        cin.ignore(1000, '\n');
                    }
                    else {
                        break;
                    }
                }

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
                }
                else {
                    cout << "Total Income:  + " << totalInc << " VND" << endl;
                    cout << "Total Expense: - " << totalExp << " VND" << endl;

                    long long net = totalInc - totalExp;
                    cout << "Net Savings:   " << (net >= 0 ? "+ " : "") << net << " VND" << endl;
                }
                pause();
                break;
            }
            case 2: statsTimeRange(); break;
            case 3: statsWalletTime(); break;
            case 4: statsAnnualOverview(); break;
            case 5: statsAnnualBreakdown(true); break;  // Income
            case 6: statsAnnualBreakdown(false); break; // Expense
            case 7: {
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
                break;
            }
            default:
                cout << "Invalid choice. Please try again." << endl;
                pause();
                break;
        }
    }
}

// RECURRING MENU FUNCTIONS

void addRecurringTask() {
    clearScreen();
    cout << "---ADD RECURRING TASK---" << endl;
    
    if (wallets.size() == 0) {
        cout << "Error: No wallets available." << endl;
        pause(); return;
    }

    // Type
    cout << "1. Income\n2. Expense\nSelect Type: ";
    int typeChoice; cin >> typeChoice;
    if (cin.fail() || (typeChoice != 1 && typeChoice != 2)) {
        cin.clear(); cin.ignore(1000, '\n'); return;
    }
    flow type = (typeChoice == 1) ? income : expense;

    // Wallet
    cout << "\nSelect Wallet to impact:" << endl;
    for (long long i = 0; i < wallets.size(); i++) {
        cout << i + 1 << ". " << wallets[i].getName() << endl;
    }
    int wIdx;
    while (true) {
        cout << "Choice: ";
        cin >> wIdx;
        if (cin.fail()) {
            cout << "Invalid input. Enter a number: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        else {
            break;
        }
    }
    if (wIdx < 1 || wIdx > wallets.size()) return;
    string wId = wallets[wIdx - 1].getId();

    // Category/Source
    string catSourceId;
    if (type == income) {
        if (incomeSources.size() == 0) { cout << "No Income Sources defined." << endl; pause(); return; }
        cout << "\nSelect Source:" << endl;
        for (long long i = 0; i < incomeSources.size(); i++) {
             cout << i + 1 << ". " << incomeSources[i].getName() << endl;
        }
        int sIdx;
        while (true) {
            cout << "Choice: ";
            cin >> sIdx;
            if (cin.fail()) {
                cout << "Invalid input. Enter a number: ";
                cin.clear();
                cin.ignore(1000, '\n');
            }
            else {
                break;
            }
        }
        if (sIdx < 1 || sIdx > incomeSources.size()) return;
        catSourceId = incomeSources[sIdx-1].getId();
    } else {
        if (expenseCategories.size() == 0) { cout << "No Categories defined." << endl; pause(); return; }
        cout << "\nSelect Category:" << endl;
        for (long long i = 0; i < expenseCategories.size(); i++) {
             cout << i + 1 << ". " << expenseCategories[i].getName() << endl;
        }
        int cIdx;
        while (true) {
            cout << "Choice: ";
            cin >> cIdx;
            if (cin.fail()) {
                cout << "Invalid input. Enter a number: ";
                cin.clear();
                cin.ignore(1000, '\n');
            }
            else {
                break;
            }
        }
        if (cIdx < 1 || cIdx > expenseCategories.size()) return;
        catSourceId = expenseCategories[cIdx-1].getId();
    }

    // Details
    long long amount;
    while (true) {
        cout << "Enter Amount: ";
        if (cin >> amount) {
            if (amount <= 0) {
                cout << "Amount must be positive!\n";
                continue;
            }
            break;
        } else {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }
    string desc;
    cout << "Enter Description: ";
    cin.ignore();
    getline(cin, desc);

    // Start Date
    cout << "\n---Start Date (first due date)---" << endl;
    int d, m, y;
    while (true) {
        cout << "Day: "; cin >> d;
        cout << "Month: "; cin >> m;
        cout << "Year: "; cin >> y;
        if (cin.fail()) {
            cout << "Invalid format. Please enter numbers for day, then month and year." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        if (m < 1 || m > 12) {
            cout << "Invalid month. Try again.\n";
            continue;
        }
        int maxDays = Date::daysInMonth(m, y);
        if (d < 1 || d > maxDays) {
            cout << "Invalid day for this month. Try again.\n";
            continue;
        }
        break;
    }
    Date startDate(d, m, y);

    // End Date
    bool hasEnd = false;
    Date endD;

    cout << "\nDo you want to set an End Date? (y/n): ";
    char endChoice;
    cin >> endChoice;

    if (endChoice == 'y' || endChoice == 'Y') {
        hasEnd = true;
        cout << "---End Date---" << endl;
        int ed, em, ey;
        while (true) {
            cout << "Day: "; cin >> ed;
            cout << "Month: "; cin >> em;
            cout << "Year: "; cin >> ey;
            if (cin.fail()) {
                cout << "Invalid format. Please enter numbers for day, then month and year." << endl;
                cin.clear();
                cin.ignore(1000, '\n');
                continue;
            }
            if (em < 1 || em > 12 || ed < 1 || ed > 31) {
                cout << "Invalid date. Try again.\n";
                continue;
            }
            Date tempEnd(ed, em, ey);
            if (startDate > tempEnd) {
                cout << "Error: End date cannot be before Start date!\n";
                continue;
            }
            endD = tempEnd;
            break;
        }
    }

    string rId = recurringTasks.generateNewId("REC");
    RecurringTask rt(rId, type, amount, desc, wId, catSourceId, startDate, hasEnd, endD);
    recurringTasks.push_back(rt);

    cout << "\nRecurring task added! It will trigger automatically on check" << endl;
    pause();
}

void removeRecurringTask() {
    if (recurringTasks.size() == 0) {
        cout << "No recurring tasks found." << endl;
        pause(); return;
    }
    
    clearScreen();
    cout << "---REMOVE RECURRING TASK---" << endl;
    for(long long i=0; i<recurringTasks.size(); i++) {
        Date next = recurringTasks[i].getNextDueDate();
        cout << i+1 << ". " << recurringTasks[i].getId() << " | Due: " 
             << next.getDay() << "/" << next.getMonth() << "/" << next.getYear() << endl;
    }
    
    cout << "\nSelect number to remove (0 to cancel): ";
    int choice;
    while (!(cin >> choice)) {
        cout << "Invalid input. Please enter a number: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    
    if (choice > 0 && choice <= recurringTasks.size()) {
        recurringTasks.remove(choice - 1);
        cout << "Task removed." << endl;
    }
    pause();
}

void viewRecurringTasks() {
    clearScreen();
    cout << "---ACTIVE RECURRING TASKS---" << endl;
    if (recurringTasks.size() == 0) {
        cout << "No active recurring tasks" << endl;
    } else {
        cout << left << setw(5) << "#" << setw(25) << "Description" << setw(15) << "Next Due" << endl;
        cout << string(50, '-') << endl;
        for(long long i=0; i<recurringTasks.size(); i++) {
            Date next = recurringTasks[i].getNextDueDate();
            cout << left << setw(5) << i+1 
                 << setw(25) << recurringTasks[i].generateTransaction("").getDescription().substr(0,24)
                 << next.getDay() << "/" << next.getMonth() << "/" << next.getYear() << endl;
        }
    }
    pause();
}

void recurringMenu() {
    while(true) {
        clearScreen();
        cout << "---RECURRING TRANSACTIONS---" << endl;
        cout << "1. Add New Recurring Task" << endl;
        cout << "2. View Active Tasks" << endl;
        cout << "3. Remove Recurring Task" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << "Choice: ";
        int c; cin >> c;
        
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        if (c == 4) break;
        
        switch(c) {
            case 1: addRecurringTask(); break;
            case 2: viewRecurringTasks(); break;
            case 3: removeRecurringTask(); break;
        }
    }
}

// SETTINGS MENU FUNCTIONS

void addWallet() {
    string name;
    cout << "Enter wallet name: ";
    cin.ignore(); 
    getline(cin, name);
    
    long long bal;
    while (true) {
        cout << "Enter initial balance: ";
        if (cin >> bal) {
            break;
        } else {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }

    string id = wallets.generateNewId("WAL");
    wallets.push_back(Wallet(id, name, bal));
    cout << "Wallet added successfully!" << endl;
}

void removeWallet() {
    if (wallets.size() == 0) {
        cout << "No wallets to remove." << endl;
        return;
    }
    clearScreen();
    cout << "---REMOVE WALLET---" << endl;
    for (long long i = 0; i < wallets.size(); i++) {
        cout << i + 1 << ". " << wallets[i].getName() << " (Balance: " << wallets[i].getBalance() << ")" << endl;
    }
    cout << "Select wallet to remove (0 to cancel): ";
    int choice;
    cin >> choice;
    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }

    if (choice > 0 && choice <= wallets.size()) {
        cout << "\nWARNING: Deleting this wallet will NOT delete transactions associated with it." << endl;
        cout << "Transactions history will show 'Unknown Wallet'." << endl;
        cout << "Any recurring tasks tied to this wallet will also be deleted." << endl;
        cout << "Are you sure? (y/n): ";
        char confirm; cin >> confirm;
        cin.ignore(1000, '\n');
        
        if (confirm == 'y' || confirm == 'Y') {
            string targetId = wallets[choice - 1].getId();

            int tasksRemoved = 0;
            for (long long i = recurringTasks.size() - 1; i >= 0; i--) {
                if (recurringTasks[i].getWalletId() == targetId) {
                    recurringTasks.remove(i);
                    tasksRemoved++;
                }
            }
            wallets.remove(choice - 1);
            
            cout << "Wallet removed." << endl;
            if (tasksRemoved > 0) {
                cout << "Also removed " << tasksRemoved << " recurring task(s) linked to this wallet." << endl;
            }
        }
    }
}

void editWallet() {
    if (wallets.size() == 0) {
        cout << "No wallets to edit." << endl;
        pause(); return;
    }
    clearScreen();
    cout << "---EDIT WALLET NAME---" << endl;
    for (long long i = 0; i < wallets.size(); i++) {
        cout << i + 1 << ". " << wallets[i].getName() << endl;
    }
    cout << "Select wallet to rename (0 to cancel): ";
    int choice; cin >> choice;

    if (cin.fail() || choice < 1 || choice > wallets.size()) {
        cin.clear(); cin.ignore(1000, '\n'); return;
    }

    string newName;
    cout << "Enter new name: ";
    cin.ignore();
    getline(cin, newName);

    if (!newName.empty()) {
        wallets[choice - 1].setName(newName);
        cout << "Wallet renamed successfully." << endl;
    }
    pause();
}

void editCategory() {
    if (expenseCategories.size() == 0) {
        cout << "No categories to edit." << endl;
        pause(); return;
    }
    clearScreen();
    cout << "---EDIT EXPENSE CATEGORY---" << endl;
    for (long long i = 0; i < expenseCategories.size(); i++) {
        cout << i + 1 << ". " << expenseCategories[i].getName() << endl;
    }
    cout << "Select category to rename (0 to cancel): ";
    int choice; cin >> choice;

    if (cin.fail() || choice < 1 || choice > expenseCategories.size()) {
        cin.clear(); cin.ignore(1000, '\n'); return;
    }

    string newName;
    cout << "Enter new name: ";
    cin.ignore();
    getline(cin, newName);

    if (!newName.empty()) {
        expenseCategories[choice - 1].setName(newName);
        cout << "Category renamed successfully." << endl;
    }
    pause();
}

void removeCategory() {
    if (expenseCategories.size() == 0) {
        cout << "No expense categories to remove." << endl;
        pause(); return;
    }
    clearScreen();
    cout << "---REMOVE EXPENSE CATEGORY---" << endl;
    for (long long i = 0; i < expenseCategories.size(); i++) {
        cout << i + 1 << ". " << expenseCategories[i].getName() << endl;
    }
    cout << "Select category to remove (0 to cancel): ";
    int choice; cin >> choice;

    if (cin.fail() || choice < 1 || choice > expenseCategories.size()) {
        cin.clear(); cin.ignore(1000, '\n'); return;
    }

    cout << "\nWARNING: Deleting this category will NOT delete transactions associated with it." << endl;
    cout << "Transactions history will show 'Unknown Category'." << endl;
    cout << "Any recurring tasks tied to this category will also be deleted." << endl;
    cout << "Are you sure? (y/n): ";
    char confirm; cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        string targetId = expenseCategories[choice - 1].getId();

        int tasksRemoved = 0;
        for (long long i = recurringTasks.size() - 1; i >= 0; i--) {
            if (recurringTasks[i].getCategoryId() == targetId) {
                recurringTasks.remove(i);
                tasksRemoved++;
            }
        }
        expenseCategories.remove(choice - 1);
        cout << "Category removed." << endl;
        if (tasksRemoved > 0) {
            cout << "Also removed " << tasksRemoved << " recurring task(s) linked to this category." << endl;
        }
        pause();
    }
}
    
void editSource() {
    if (incomeSources.size() == 0) {
        cout << "No sources to edit." << endl;
        pause(); return;
    }
    clearScreen();
    cout << "---EDIT INCOME SOURCE---" << endl;
    for (long long i = 0; i < incomeSources.size(); i++) {
        cout << i + 1 << ". " << incomeSources[i].getName() << endl;
    }
    cout << "Select source to rename (0 to cancel): ";
    int choice; cin >> choice;

    if (cin.fail() || choice < 1 || choice > incomeSources.size()) {
        cin.clear(); cin.ignore(1000, '\n'); return;
    }

    string newName;
    cout << "Enter new name: ";
    cin.ignore();
    getline(cin, newName);

    if (!newName.empty()) {
        incomeSources[choice - 1].setName(newName);
        cout << "Source renamed successfully." << endl;
    }
    pause();
}

void removeSource() {
    if (incomeSources.size() == 0) {
        cout << "No sources to remove." << endl;
        pause(); return;
    }
    clearScreen();
    cout << "---REMOVE INCOME SOURCE---" << endl;
    for (long long i = 0; i < incomeSources.size(); i++) {
        cout << i + 1 << ". " << incomeSources[i].getName() << endl;
    }
    cout << "Select source to remove (0 to cancel): ";
    int choice; cin >> choice;

    if (cin.fail() || choice < 1 || choice > incomeSources.size()) {
        cin.clear(); cin.ignore(1000, '\n'); return;
    }
    cout << "\nWARNING: Deleting this source will NOT delete transactions associated with it." << endl;
    cout << "Transactions history will show 'Unknown Category'." << endl;
    cout << "Any recurring tasks tied to this source will also be deleted." << endl;
    cout << "Are you sure? (y/n): ";
    char confirm; cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        string targetId = incomeSources[choice - 1].getId();
        int tasksRemoved = 0;
        for (long long i = recurringTasks.size() - 1; i >= 0; i--) {
            if (recurringTasks[i].getCategoryId() == targetId) {
                recurringTasks.remove(i);
                tasksRemoved++;
            }
        }
        incomeSources.remove(choice - 1);
        cout << "Source removed." << endl;
        if (tasksRemoved > 0) {
            cout << "Also removed " << tasksRemoved << " recurring task(s) linked to this source." << endl;
        }
        pause();
    }
}

void addCategory() {
    string name;
    cout << "Enter Expense Category: ";
    cin.ignore();
    getline(cin, name);
    string id = expenseCategories.generateNewId("CAT");
    expenseCategories.push_back(ExpenseCategory(id, name));
    cout << "Category added successfully!" << endl;
}

void addSource() {
    string name;
    cout << "Enter Income Source: ";
    cin.ignore();
    getline(cin, name);
    string id = incomeSources.generateNewId("SRC");
    incomeSources.push_back(IncomeSource(id, name));
    cout << "Source added successfully!" << endl;
}

// Add transaction menu
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

    cout << "\n---Select Wallet---" << endl;
    for (long long i = 0; i < wallets.size(); i++) {
        cout << i + 1 << ". " << wallets[i].getName() << " (Bal: " << wallets[i].getBalance() << ")" << endl;
    }
    int wIdx;
    cout << "Choice: ";
    while (!(cin >> wIdx)) {
        cout << "Invalid input. Please enter a number: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }

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
        cout << "Choice: ";
        while (!(cin >> sIdx)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        if (sIdx < 1 || sIdx > incomeSources.size()) return;
        catSourceId = incomeSources[sIdx - 1].getId();
    } else {
        if (expenseCategories.size() == 0) { cout << "No Expense Categories defined" << endl; pause(); return; }
        for (long long i = 0; i < expenseCategories.size(); i++) {
            cout << i + 1 << ". " << expenseCategories[i].getName() << endl;
        }
        int cIdx;
        cout << "Choice: ";
        while (!(cin >> cIdx)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        if (cIdx < 1 || cIdx > expenseCategories.size()) return;
        catSourceId = expenseCategories[cIdx - 1].getId();
    }

    long long amount;
    while (true) {
        cout << "\nEnter Amount: ";
        if (cin >> amount) {
            // Prevent negative amount
            if (amount <= 0) {
                cout << "Amount must be positive!" << endl;
                continue;
            }
            // Prevent overdraft
            if (type == expense) {
                long long currentBal = wallets[wIdx - 1].getBalance();
                if (amount > currentBal) {
                    cout << "Error: Insufficient funds! (Balance: " << currentBal << ")" << endl;
                    continue;
                }
            }
            break;
        }
        else {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear(); cin.ignore(1000, '\n');
        }
    }
    
    string desc;
    cout << "Enter description: ";
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

    cout << "\nTransaction added successfully!" << endl;
    pause();
}

void settingsMenu() {
    while (true) {
        clearScreen();
        cout << "---MASTER DATA SETTINGS---" << endl;
        cout << "1. Add new Wallet" << endl;
        cout << "2. Edit Wallet Name" << endl;
        cout << "3. Remove Wallet" << endl;
        cout << "------------------" << endl;
        cout << "4. Add Expense Category" << endl;
        cout << "5. Edit Category" << endl;
        cout << "6. Remove Category" << endl;
        cout << "------------------" << endl;
        cout << "7. Add Income Source" << endl;
        cout << "8. Edit Source" << endl;
        cout << "9. Remove Source" << endl;
        cout << "------------------" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Choice: ";
        int c; cin >> c;
        
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (c == 0) break;

        switch (c) {
        case 1: addWallet(); break;
        case 2: editWallet(); break;
        case 3: removeWallet(); break;

        case 4: addCategory(); break;
        case 5: editCategory(); break;
        case 6: removeCategory(); break;

        case 7: addSource(); break;
        case 8: editSource(); break;
        case 9: removeSource(); break;
        }
        if (c == 1 || c == 3 || c == 4 || c == 7) pause();
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
        cout << "1. Dashboard" << endl;
        cout << "2. Add Transaction" << endl;
        cout << "3. Recurring Transactions" << endl;
        cout << "4. Statistics & Reports" << endl;
        cout << "5. Settings (Wallets/Categories)" << endl;
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
            recurringMenu();
            break;
        case 4:
            statisticsMenu();
            break;
        case 5:
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