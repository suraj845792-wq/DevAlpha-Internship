/*
 * ============================================================
 *  DevAlpha Technologies — C++ Programming Internship
 *  Task 1 (Easy): OOP Based Program
 *  Project     : Bank Account Management System
 *  Concepts    : Classes, Inheritance, Encapsulation,
 *                Polymorphism, Abstraction, File I/O
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <stdexcept>

using namespace std;

// ---------------------------------------------
//  UTILITY: Get current date-time as string
// ---------------------------------------------
string getCurrentDateTime() {
    time_t now = time(0);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

// ---------------------------------------------
//  CLASS: Transaction  (Encapsulation)
// ---------------------------------------------
class Transaction {
private:
    string type;
    double amount;
    string dateTime;

public:
    Transaction(string t, double amt)
        : type(t), amount(amt), dateTime(getCurrentDateTime()) {}

    void display() const {
        cout << "  [" << dateTime << "]  "
             << left << setw(12) << type
             << "  Rs. " << fixed << setprecision(2) << amount << endl;
    }
};

// ---------------------------------------------
//  ABSTRACT BASE CLASS: Account  (Abstraction)
// ---------------------------------------------
class Account {
protected:
    int accountNumber;
    string holderName;
    double balance;
    vector<Transaction> history;

public:
    Account(int accNo, string name, double initialBalance)
        : accountNumber(accNo), holderName(name), balance(initialBalance) {
        history.push_back(Transaction("Open Acct", initialBalance));
    }

    // Pure virtual — must be overridden (Abstraction)
    virtual string getAccountType() const = 0;
    virtual double getInterestRate()  const = 0;

    // Deposit money
    void deposit(double amount) {
        if (amount <= 0)
            throw invalid_argument("Deposit amount must be positive.");
        balance += amount;
        history.push_back(Transaction("Deposit", amount));
        cout << "\n  [OK] Deposited Rs. " << fixed << setprecision(2) << amount
             << " | New Balance: Rs. " << balance << endl;
    }

    // Withdraw money (virtual so child classes can override limits)
    virtual void withdraw(double amount) {
        if (amount <= 0)
            throw invalid_argument("Withdrawal amount must be positive.");
        if (amount > balance)
            throw runtime_error("Insufficient funds.");
        balance -= amount;
        history.push_back(Transaction("Withdraw", amount));
        cout << "\n  [OK] Withdrawn Rs. " << fixed << setprecision(2) << amount
             << " | New Balance: Rs. " << balance << endl;
    }

    // Apply monthly interest
    void applyInterest() {
        double interest = balance * getInterestRate() / 100.0;
        balance += interest;
        history.push_back(Transaction("Interest", interest));
        cout << "\n  [OK] Interest of Rs. " << fixed << setprecision(2) << interest
             << " applied | New Balance: Rs. " << balance << endl;
    }

    // Display account details (Polymorphism via virtual)
    virtual void displayInfo() const {
        cout << "\n  +-------------------------------------+" << endl;
        cout << "  |       ACCOUNT DETAILS               |" << endl;
        cout << "  +-------------------------------------+" << endl;
        cout << "  | Account No   : " << accountNumber       << endl;
        cout << "  | Holder Name  : " << holderName          << endl;
        cout << "  | Account Type : " << getAccountType()    << endl;
        cout << "  | Balance      : Rs. " << fixed << setprecision(2) << balance << endl;
        cout << "  | Interest Rate: " << getInterestRate() << "%" << endl;
        cout << "  +-------------------------------------+" << endl;
    }

    // Print transaction history
    void printHistory() const {
        cout << "\n  -- Transaction History for Acc #" << accountNumber << " --" << endl;
        if (history.empty()) {
            cout << "  No transactions yet." << endl;
            return;
        }
        for (const auto& t : history)
            t.display();
    }

    // Getters
    int    getAccountNumber() const { return accountNumber; }
    string getHolderName()    const { return holderName; }
    double getBalance()       const { return balance; }

    virtual ~Account() {}
};

// ---------------------------------------------
//  DERIVED CLASS: SavingsAccount  (Inheritance)
// ---------------------------------------------
class SavingsAccount : public Account {
private:
    static const double INTEREST_RATE;
    static const double MIN_BALANCE;

public:
    SavingsAccount(int accNo, string name, double initialBalance)
        : Account(accNo, name, initialBalance) {
        if (initialBalance < MIN_BALANCE)
            throw invalid_argument("Savings account requires minimum Rs. 1000.");
    }

    string getAccountType() const override { return "Savings Account"; }
    double getInterestRate() const override { return INTEREST_RATE; }

    // Override: enforce minimum balance on withdrawal
    void withdraw(double amount) override {
        if (balance - amount < MIN_BALANCE)
            throw runtime_error("Cannot withdraw: minimum balance of Rs. 1000 must be maintained.");
        Account::withdraw(amount);
    }

    void displayInfo() const override {
        Account::displayInfo();
        cout << "  | Min Balance  : Rs. " << MIN_BALANCE << endl;
    }
};

const double SavingsAccount::INTEREST_RATE = 4.0;
const double SavingsAccount::MIN_BALANCE   = 1000.0;

// ---------------------------------------------
//  DERIVED CLASS: CurrentAccount  (Inheritance)
// ---------------------------------------------
class CurrentAccount : public Account {
private:
    static const double INTEREST_RATE;
    double overdraftLimit;

public:
    CurrentAccount(int accNo, string name, double initialBalance, double overdraft = 5000.0)
        : Account(accNo, name, initialBalance), overdraftLimit(overdraft) {}

    string getAccountType() const override { return "Current Account"; }
    double getInterestRate() const override { return INTEREST_RATE; }

    // Override: allow overdraft
    void withdraw(double amount) override {
        if (amount > balance + overdraftLimit)
            throw runtime_error("Exceeds overdraft limit.");
        balance -= amount;
        history.push_back(Transaction("Withdraw", amount));
        cout << "\n  [OK] Withdrawn Rs. " << fixed << setprecision(2) << amount
             << " | New Balance: Rs. " << balance << endl;
    }

    void displayInfo() const override {
        Account::displayInfo();
        cout << "  | Overdraft Lmt: Rs. " << overdraftLimit << endl;
    }
};

const double CurrentAccount::INTEREST_RATE = 2.0;

// ---------------------------------------------
//  CLASS: Bank  (Manager / Controller)
// ---------------------------------------------
class Bank {
private:
    string bankName;
    vector<Account*> accounts;
    int nextAccNo;

    Account* findAccount(int accNo) {
        for (auto* acc : accounts)
            if (acc->getAccountNumber() == accNo)
                return acc;
        return nullptr;
    }

public:
    Bank(string name) : bankName(name), nextAccNo(1001) {}

    ~Bank() {
        for (auto* acc : accounts)
            delete acc;
    }

    // Create savings account
    void createSavingsAccount(string name, double deposit) {
        try {
            Account* acc = new SavingsAccount(nextAccNo++, name, deposit);
            accounts.push_back(acc);
            cout << "\n  [OK] Savings Account created! Account No: "
                 << acc->getAccountNumber() << endl;
        } catch (exception& e) {
            cout << "\n  [ERR] Error: " << e.what() << endl;
        }
    }

    // Create current account
    void createCurrentAccount(string name, double deposit) {
        Account* acc = new CurrentAccount(nextAccNo++, name, deposit);
        accounts.push_back(acc);
        cout << "\n  [OK] Current Account created! Account No: "
             << acc->getAccountNumber() << endl;
    }

    void deposit(int accNo, double amount) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "\n  [ERR] Account not found." << endl; return; }
        try { acc->deposit(amount); }
        catch (exception& e) { cout << "\n  [ERR] " << e.what() << endl; }
    }

    void withdraw(int accNo, double amount) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "\n  [ERR] Account not found." << endl; return; }
        try { acc->withdraw(amount); }
        catch (exception& e) { cout << "\n  [ERR] " << e.what() << endl; }
    }

    void applyInterest(int accNo) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "\n  [ERR] Account not found." << endl; return; }
        acc->applyInterest();
    }

    void showAccount(int accNo) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "\n  [ERR] Account not found." << endl; return; }
        acc->displayInfo();         // Polymorphism in action
    }

    void showHistory(int accNo) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "\n  [ERR] Account not found." << endl; return; }
        acc->printHistory();
    }

    void listAllAccounts() const {
        cout << "\n  == All Accounts at " << bankName << " ==" << endl;
        if (accounts.empty()) { cout << "  No accounts found." << endl; return; }
        cout << "  " << left << setw(10) << "Acc No"
             << setw(20) << "Holder"
             << setw(22) << "Type"
             << "Balance" << endl;
        cout << "  " << string(60, '-') << endl;
        for (const auto* acc : accounts) {
            cout << "  " << setw(10) << acc->getAccountNumber()
                 << setw(20) << acc->getHolderName()
                 << setw(22) << acc->getAccountType()
                 << "Rs. " << fixed << setprecision(2) << acc->getBalance() << endl;
        }
    }
};

// ---------------------------------------------
//  MAIN: Menu-driven interface
// ---------------------------------------------
int main() {
    Bank bank("DevAlpha National Bank");

    cout << "\n  +======================================+" << endl;
    cout << "  |   DevAlpha National Bank System      |" << endl;
    cout << "  +======================================+" << endl;

    int choice;
    do {
        cout << "\n  ------- MAIN MENU -------" << endl;
        cout << "  1. Create Savings Account" << endl;
        cout << "  2. Create Current Account" << endl;
        cout << "  3. Deposit Money"           << endl;
        cout << "  4. Withdraw Money"          << endl;
        cout << "  5. Apply Monthly Interest"  << endl;
        cout << "  6. View Account Details"    << endl;
        cout << "  7. View Transaction History"<< endl;
        cout << "  8. List All Accounts"       << endl;
        cout << "  0. Exit"                    << endl;
        cout << "  Enter choice: ";
        cin  >> choice;

        if (choice == 1 || choice == 2) {
            string name; double amount;
            cin.ignore();
            cout << "  Enter holder name  : "; getline(cin, name);
            cout << "  Enter initial deposit: Rs. "; cin >> amount;
            if (choice == 1) bank.createSavingsAccount(name, amount);
            else             bank.createCurrentAccount(name, amount);

        } else if (choice == 3 || choice == 4) {
            int accNo; double amount;
            cout << "  Enter account number : "; cin >> accNo;
            cout << "  Enter amount         : Rs. "; cin >> amount;
            if (choice == 3) bank.deposit(accNo, amount);
            else             bank.withdraw(accNo, amount);

        } else if (choice == 5 || choice == 6 || choice == 7) {
            int accNo;
            cout << "  Enter account number : "; cin >> accNo;
            if      (choice == 5) bank.applyInterest(accNo);
            else if (choice == 6) bank.showAccount(accNo);
            else                  bank.showHistory(accNo);

        } else if (choice == 8) {
            bank.listAllAccounts();

        } else if (choice != 0) {
            cout << "  Invalid choice. Try again." << endl;
        }

    } while (choice != 0);

    cout << "\n  Thank you for using DevAlpha National Bank!\n" << endl;
    return 0;
}