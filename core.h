<<<<<<< HEAD
#pragma once
=======
#pragma once  
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include "models.h"

// Менеджер валют (Singleton)
class CurrencyManager {
private:
    std::unordered_map<std::string, double> exchangeRates;
    std::string baseCurrency;
    CurrencyManager();
public:
    static CurrencyManager& getInstance();
    CurrencyManager(const CurrencyManager&) = delete;
    CurrencyManager& operator=(const CurrencyManager&) = delete;
<<<<<<< HEAD
    double convert(double amount, const std::string& from, const std::string& to);
=======

    double convert(double amount, const std::string& from, const std::string& to);
    void setExchangeRate(const std::string& currency, double newRate);
    void printRates() const;
    std::unordered_map<std::string, double> getRates() const; // Додано для меню управління курсами
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
};

// Менеджер рахунків
class AccountManager {
private:
    std::vector<std::shared_ptr<Account>> accounts;
    int transactionCounter = 1;
    int accountCounter = 1;
    std::string generateTxId();
public:
    std::string generateAccId();
    void setCounters(int accCount, int txCount);
    void addAccount(std::shared_ptr<Account> account);
    std::vector<std::shared_ptr<Account>> getAccounts() const;
    std::shared_ptr<Account> getAccountById(const std::string& id);
<<<<<<< HEAD
=======

    bool deleteAccount(const std::string& accountId, const std::string& userName);
    bool updateAccount(const std::string& accountId, const std::string& userName, const std::string& newName, double newLimit = -1.0);

    // Метод для переказів між рахунками
    bool transferFunds(const std::string& fromId, const std::string& toId, double amount, const std::string& date, const std::string& userName);

>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
    bool makeExpense(const std::string& accountId, double amount, const std::string& category, const std::string& description, std::string date, std::string userName);
    bool makeIncome(const std::string& accountId, double amount, const std::string& category, const std::string& description, std::string date, std::string userName);
    std::vector<Transaction> getTransactionsForUser(const std::string& userName) const;
};

// Робота з файлами
class StorageManager {
public:
    static void saveToFile(const AccountManager& manager, const std::string& filename);
    static void loadFromFile(AccountManager& manager, const std::string& filename);
};

// Звіти
class ReportGenerator {
public:
    static std::vector<Transaction> getTop3Expenses(const std::vector<Transaction>& history, const std::string& startDate, const std::string& endDate);
    static std::map<std::string, double> getExpensesByUser(const std::vector<Transaction>& history, const std::string& startDate, const std::string& endDate);
<<<<<<< HEAD
};
=======

    // Новий звіт за категоріями
    static std::vector<std::pair<std::string, double>> getTop3Categories(const std::vector<Transaction>& history, const std::string& startDate, const std::string& endDate);
};
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
