#include "core.h"
#include "utils.h"
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

// --- AccountManager ---
string AccountManager::generateTxId() { return to_string(transactionCounter++); }
string AccountManager::generateAccId() { return to_string(accountCounter++); }

void AccountManager::setCounters(int accCount, int txCount) {
    accountCounter = accCount;
    transactionCounter = txCount;
}

void AccountManager::addAccount(shared_ptr<Account> account) { accounts.push_back(account); }
vector<shared_ptr<Account>> AccountManager::getAccounts() const { return accounts; }

shared_ptr<Account> AccountManager::getAccountById(const string& id) {
    for (auto& acc : accounts) { if (acc->getId() == id) return acc; }
    return nullptr;
}

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
static vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) tokens.push_back(token);
    return tokens;
}

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
                out << members[i];
                if (i < members.size() - 1) out << ",";
            }
        }
        out << "\n";

        for (const auto& tx : acc->getHistory()) {
            out << "TX|" << tx.id << "|" << tx.amount << "|" << tx.category << "|"
                << tx.date << "|" << tx.description << "|" << tx.isIncome << "|" << tx.userName << "\n";
        }
    }
    out.close();
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

            if (type == "WALLET") {
                currentAcc = make_shared<Wallet>(id, name, curr, owner, balance);
            }
            else if (type == "CREDIT_CARD") {
                currentAcc = make_shared<CreditCard>(id, name, curr, owner, stod(parts[7]), balance);
            }
            else if (type == "SHARED_BUDGET") {
                currentAcc = make_shared<SharedBudget>(id, name, curr, split(parts[7], ','), balance);
            }
            manager.addAccount(currentAcc);
        }
        else if (parts[0] == "TX" && currentAcc != nullptr) {
            Transaction tx{ parts[1], stod(parts[2]), parts[3], parts[4], parts[5], (parts[6] == "1"), parts[7] };
            maxTxId = max(maxTxId, stoi(tx.id));
            currentAcc->addTransaction(tx);
        }
    }
    in.close();
    manager.setCounters(maxAccId + 1, maxTxId + 1);
}

// --- ReportGenerator ---
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
}
