#include "core.h"
#include "utils.h"
<<<<<<< HEAD
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

// --- CurrencyManager ---
CurrencyManager::CurrencyManager() {
    baseCurrency = "UAH";
    exchangeRates["UAH"] = 1.0;
    exchangeRates["USD"] = 38.5;
    exchangeRates["EUR"] = 42.0;
=======
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;

// Допоміжна функція для розбиття рядка (обмежена цим файлом)
namespace {
    vector<string> split(const string& s, char delimiter) {
        vector<string> tokens;
        string token;
        istringstream tokenStream(s);
        while (getline(tokenStream, token, delimiter)) tokens.push_back(token);
        return tokens;
    }
}

// --- CurrencyManager ---

CurrencyManager::CurrencyManager() {
    baseCurrency = "UAH";
    exchangeRates["UAH"] = 1.0;
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
}

CurrencyManager& CurrencyManager::getInstance() {
    static CurrencyManager instance;
    return instance;
}

double CurrencyManager::convert(double amount, const string& from, const string& to) {
    if (exchangeRates.find(from) == exchangeRates.end() || exchangeRates.find(to) == exchangeRates.end()) {
        throw invalid_argument("Невідома валюта!");
    }
    return (amount * exchangeRates[from]) / exchangeRates[to];
}

<<<<<<< HEAD
// --- AccountManager ---
=======
void CurrencyManager::setExchangeRate(const string& currency, double newRate) {
    if (newRate > 0) {
        exchangeRates[currency] = newRate;
        cout << "[Система] Курс для " << currency << " оновлено: " << newRate << "\n";
    }
}

void CurrencyManager::printRates() const {
    cout << "--- Поточні курси (базова " << baseCurrency << ") ---\n";
    for (const auto& pair : exchangeRates) {
        cout << pair.first << ": " << pair.second << "\n";
    }
}

unordered_map<string, double> CurrencyManager::getRates() const {
    return exchangeRates;
}

// --- AccountManager ---

>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
string AccountManager::generateTxId() { return to_string(transactionCounter++); }
string AccountManager::generateAccId() { return to_string(accountCounter++); }

void AccountManager::setCounters(int accCount, int txCount) {
    accountCounter = accCount;
    transactionCounter = txCount;
}

void AccountManager::addAccount(shared_ptr<Account> account) { accounts.push_back(account); }
<<<<<<< HEAD
=======

>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
vector<shared_ptr<Account>> AccountManager::getAccounts() const { return accounts; }

shared_ptr<Account> AccountManager::getAccountById(const string& id) {
    for (auto& acc : accounts) { if (acc->getId() == id) return acc; }
    return nullptr;
}

<<<<<<< HEAD
=======
bool AccountManager::deleteAccount(const string& accountId, const string& userName) {
    auto it = find_if(accounts.begin(), accounts.end(), [&](const shared_ptr<Account>& acc) {
        return acc->getId() == accountId;
        });

    if (it == accounts.end()) {
        cout << "[Система] Помилка: Рахунок не існує.\n";
        return false;
    }

    if ((*it)->getOwner() != userName) {
        cout << "[Система] Доступ заборонено: Ви не власник.\n";
        return false;
    }

    accounts.erase(it);
    cout << "[Система] Рахунок успішно видалено.\n";
    return true;
}

bool AccountManager::updateAccount(const string& accountId, const string& userName, const string& newName, double newLimit) {
    auto acc = getAccountById(accountId);
    if (!acc || acc->getOwner() != userName) {
        cout << "[Система] Помилка доступу або рахунок не знайдено.\n";
        return false;
    }

    if (!newName.empty()) acc->setName(sanitize(newName));

    if (acc->getType() == "CREDIT_CARD") {
        auto cc = dynamic_pointer_cast<CreditCard>(acc);
        if (cc && newLimit >= 0) cc->setLimit(newLimit);
    }

    cout << "[Система] Дані оновлено.\n";
    return true;
}

bool AccountManager::transferFunds(const string& fromId, const string& toId, double amount, const string& date, const string& userName) {
    auto fromAcc = getAccountById(fromId);
    auto toAcc = getAccountById(toId);

    if (!fromAcc || !toAcc || !fromAcc->hasAccess(userName)) return false;

    if (fromAcc->withdraw(amount)) {
        double finalAmount = amount;
        if (fromAcc->getCurrency() != toAcc->getCurrency()) {
            finalAmount = CurrencyManager::getInstance().convert(amount, fromAcc->getCurrency(), toAcc->getCurrency());
        }
        toAcc->deposit(finalAmount);

        fromAcc->addTransaction({ generateTxId(), amount, "Transfer", date, "To: " + toId, false, userName });
        toAcc->addTransaction({ generateTxId(), finalAmount, "Transfer", date, "From: " + fromId, true, userName });
        return true;
    }
    return false;
}

>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
bool AccountManager::makeExpense(const string& accountId, double amount, const string& category, const string& description, string date, string userName) {
    auto acc = getAccountById(accountId);
    if (acc && acc->withdraw(amount)) {
        acc->addTransaction({ generateTxId(), amount, sanitize(category), date, sanitize(description), false, sanitize(userName) });
        return true;
    }
    return false;
}

bool AccountManager::makeIncome(const string& accountId, double amount, const string& category, const string& description, string date, string userName) {
    auto acc = getAccountById(accountId);
    if (acc) {
        acc->deposit(amount);
        acc->addTransaction({ generateTxId(), amount, sanitize(category), date, sanitize(description), true, sanitize(userName) });
        return true;
    }
    return false;
}

vector<Transaction> AccountManager::getTransactionsForUser(const string& userName) const {
    vector<Transaction> userTx;
    for (const auto& acc : accounts) {
        if (acc->hasAccess(userName)) {
            auto hist = acc->getHistory();
            userTx.insert(userTx.end(), hist.begin(), hist.end());
        }
    }
    return userTx;
}

// --- StorageManager ---
<<<<<<< HEAD
static vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) tokens.push_back(token);
    return tokens;
}
=======
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)

void StorageManager::saveToFile(const AccountManager& manager, const string& filename) {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (const auto& acc : manager.getAccounts()) {
        out << "ACC|" << acc->getType() << "|" << acc->getId() << "|"
            << acc->getName() << "|" << acc->getCurrency() << "|"
            << acc->getOwner() << "|" << acc->getBalance();

        if (acc->getType() == "CREDIT_CARD") {
            auto cc = dynamic_pointer_cast<CreditCard>(acc);
            out << "|" << cc->getLimit();
        }
        else if (acc->getType() == "SHARED_BUDGET") {
            auto sb = dynamic_pointer_cast<SharedBudget>(acc);
            auto members = sb->getMembers();
            out << "|";
            for (size_t i = 0; i < members.size(); ++i) {
<<<<<<< HEAD
                out << members[i];
                if (i < members.size() - 1) out << ",";
=======
                out << members[i] << (i < members.size() - 1 ? "," : "");
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
            }
        }
        out << "\n";

        for (const auto& tx : acc->getHistory()) {
            out << "TX|" << tx.id << "|" << tx.amount << "|" << tx.category << "|"
                << tx.date << "|" << tx.description << "|" << tx.isIncome << "|" << tx.userName << "\n";
        }
    }
<<<<<<< HEAD
    out.close();
=======
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
}

void StorageManager::loadFromFile(AccountManager& manager, const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    shared_ptr<Account> currentAcc = nullptr;
    int maxAccId = 0, maxTxId = 0;

    while (getline(in, line)) {
        if (line.empty()) continue;
        vector<string> parts = split(line, '|');

        if (parts[0] == "ACC") {
            string type = parts[1], id = parts[2], name = parts[3], curr = parts[4], owner = parts[5];
            double balance = stod(parts[6]);
            maxAccId = max(maxAccId, stoi(id));

<<<<<<< HEAD
            if (type == "WALLET") {
                currentAcc = make_shared<Wallet>(id, name, curr, owner, balance);
            }
            else if (type == "CREDIT_CARD") {
                currentAcc = make_shared<CreditCard>(id, name, curr, owner, stod(parts[7]), balance);
            }
            else if (type == "SHARED_BUDGET") {
                currentAcc = make_shared<SharedBudget>(id, name, curr, split(parts[7], ','), balance);
            }
=======
            if (type == "WALLET") currentAcc = make_shared<Wallet>(id, name, curr, owner, balance);
            else if (type == "CREDIT_CARD") currentAcc = make_shared<CreditCard>(id, name, curr, owner, stod(parts[7]), balance);
            else if (type == "SHARED_BUDGET") currentAcc = make_shared<SharedBudget>(id, name, curr, split(parts[7], ','), balance);

>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
            manager.addAccount(currentAcc);
        }
        else if (parts[0] == "TX" && currentAcc != nullptr) {
            Transaction tx{ parts[1], stod(parts[2]), parts[3], parts[4], parts[5], (parts[6] == "1"), parts[7] };
            maxTxId = max(maxTxId, stoi(tx.id));
            currentAcc->addTransaction(tx);
        }
    }
<<<<<<< HEAD
    in.close();
=======
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
    manager.setCounters(maxAccId + 1, maxTxId + 1);
}

// --- ReportGenerator ---
<<<<<<< HEAD
=======

>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
vector<Transaction> ReportGenerator::getTop3Expenses(const vector<Transaction>& history, const string& startDate, const string& endDate) {
    vector<Transaction> filtered;
    for (const auto& t : history) {
        if (!t.isIncome && t.date >= startDate && t.date <= endDate) filtered.push_back(t);
    }
    sort(filtered.begin(), filtered.end(), [](const Transaction& a, const Transaction& b) { return a.amount > b.amount; });
    if (filtered.size() > 3) filtered.resize(3);
    return filtered;
}

map<string, double> ReportGenerator::getExpensesByUser(const vector<Transaction>& history, const string& startDate, const string& endDate) {
    map<string, double> userStats;
    for (const auto& t : history) {
        if (!t.isIncome && t.date >= startDate && t.date <= endDate) {
            userStats[t.userName] += t.amount;
        }
    }
    return userStats;
<<<<<<< HEAD
}
=======
}
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
