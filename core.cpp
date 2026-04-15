#include "core.h"
#include "utils.h"
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
    exchangeRates["USD"] = 43.9;
    exchangeRates["EUR"] = 51.0;
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

double CurrencyManager::getRate(const string& currency) const {
    auto it = exchangeRates.find(currency);
    if (it != exchangeRates.end()) {
        return it->second;
    }
    throw invalid_argument("Невідома валюта: " + currency);
}

void CurrencyManager::updateRate(const string& currency, double newRate) {
    if (newRate > 0) {
        exchangeRates[currency] = newRate;
        cout << "[Система] Курс для " << currency << " оновлено: " << newRate << "\n";
    } else {
        cout << "[Система] Помилка: Курс має бути додатнім.\n";
    }
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

bool AccountManager::makeTransfer(const string& fromId, const string& toId, double amount, string date, string userName) {
    auto accFrom = getAccountById(fromId);
    auto accTo = getAccountById(toId);
    
    if (!accFrom || !accTo || accFrom == accTo) return false;
    
    if (accFrom->withdraw(amount)) {
        double convertedAmount = amount;
        // Якщо валюти різні, автоматично конвертуємо за актуальним курсом
        if (accFrom->getCurrency() != accTo->getCurrency()) {
            convertedAmount = CurrencyManager::getInstance().convert(amount, accFrom->getCurrency(), accTo->getCurrency());
        }
        
        accTo->deposit(convertedAmount);
        
        // Записуємо зі спеціальними категоріями, щоб вони не потрапляли у звичайні звіти витрат
        accFrom->addTransaction({generateTxId(), amount, "Transfer Out", date, "To ID " + toId, false, sanitize(userName)});
        accTo->addTransaction({generateTxId(), convertedAmount, "Transfer In", date, "From ID " + fromId, true, sanitize(userName)});
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
                out << members[i] << (i < members.size() - 1 ? "," : "");
            }
        }
        out << "\n";

        for (const auto& tx : acc->getHistory()) {
            out << "TX|" << tx.id << "|" << tx.amount << "|" << tx.category << "|"
                << tx.date << "|" << tx.description << "|" << tx.isIncome << "|" << tx.userName << "\n";
        }
    }
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

        if (parts[0] == "ACC" && parts.size() >= 7) {
            string type = parts[1], id = parts[2], name = parts[3], curr = parts[4], owner = parts[5];
            double balance = stod(parts[6]);
            maxAccId = max(maxAccId, stoi(id));

            if (type == "WALLET") currentAcc = make_shared<Wallet>(id, name, curr, owner, balance);
            else if (type == "CREDIT_CARD" && parts.size() >= 8) currentAcc = make_shared<CreditCard>(id, name, curr, owner, stod(parts[7]), balance);
            else if (type == "SHARED_BUDGET" && parts.size() >= 8) currentAcc = make_shared<SharedBudget>(id, name, curr, split(parts[7], ','), balance);
            else currentAcc = nullptr;

            if (currentAcc) manager.addAccount(currentAcc);
        }
        else if (parts[0] == "TX" && currentAcc != nullptr && parts.size() >= 8) {
            Transaction tx{ parts[1], stod(parts[2]), parts[3], parts[4], parts[5], (parts[6] == "1"), parts[7] };
            maxTxId = max(maxTxId, stoi(tx.id));
            currentAcc->addTransaction(tx);
        }
    }
    manager.setCounters(maxAccId + 1, maxTxId + 1);
}

// --- ReportGenerator ---
vector<Transaction> ReportGenerator::getTop3Expenses(const vector<Transaction>& history, const string& startDate, const string& endDate) {
    vector<Transaction> filtered;
    for (const auto& t : history) {
        if (!t.isIncome && t.category != "Transfer" && t.category != "Transfer Out" && t.category != "Transfer In" && t.date >= startDate && t.date <= endDate) {
            filtered.push_back(t);
        }
    }
    sort(filtered.begin(), filtered.end(), [](const Transaction& a, const Transaction& b) { return a.amount > b.amount; });
    if (filtered.size() > 3) filtered.resize(3);
    return filtered;
}

map<string, double> ReportGenerator::getExpensesByUser(const vector<Transaction>& history, const string& startDate, const string& endDate) {
    map<string, double> userStats;
    for (const auto& t : history) {
        if (!t.isIncome && t.category != "Transfer" && t.category != "Transfer Out" && t.category != "Transfer In" && t.date >= startDate && t.date <= endDate) {
            userStats[t.userName] += t.amount;
        }
    }
    return userStats;
}

std::vector<std::pair<std::string, double>> ReportGenerator::getTop3Categories(
    const std::vector<Transaction>& history,
    const std::string& startDate,
    const std::string& endDate)
{
    std::map<std::string, double> categoryTotals;

    for (const auto& t : history) {
        if (!t.isIncome && t.category != "Transfer" && t.category != "Transfer Out" && t.category != "Transfer In" &&
            t.date >= startDate && t.date <= endDate) {
            categoryTotals[t.category] += t.amount;
        }
    }

    std::vector<std::pair<std::string, double>> sortedCategories(
        categoryTotals.begin(), categoryTotals.end());

    std::sort(sortedCategories.begin(), sortedCategories.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

    if (sortedCategories.size() > 3)
        sortedCategories.resize(3);

    return sortedCategories;
}

// --- SavingsManager ---

string SavingsManager::generateGoalId() { return "G" + to_string(goalCounter++); }
void SavingsManager::setCounter(int count) { goalCounter = count; }
void SavingsManager::addGoal(const SavingsGoal& goal) { goals.push_back(goal); }
vector<SavingsGoal>& SavingsManager::getGoals() { return goals; }
const vector<SavingsGoal>& SavingsManager::getGoals() const { return goals; }

SavingsGoal* SavingsManager::getGoalById(const string& id) {
    for (auto& g : goals) {
        if (g.getId() == id) return &g;
    }
    return nullptr;
}

bool SavingsManager::deleteGoal(const string& id, const string& userName) {
    auto it = find_if(goals.begin(), goals.end(), [&](const SavingsGoal& g) { return g.getId() == id; });
    if (it == goals.end()) return false;
    if (it->getOwner() != userName) return false;
    goals.erase(it);
    return true;
}

// --- StorageManager (Savings) ---

void StorageManager::saveSavingsToFile(const SavingsManager& savings, const string& filename) {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (const auto& goal : savings.getGoals()) {
        out << "GOAL|" << goal.getId() << "|" << goal.getName() << "|"
            << goal.getTargetAmount() << "|" << goal.getCurrentAmount() << "|"
            << goal.getCurrency() << "|" << goal.getOwner() << "|"
            << goal.getDeadline() << "|" << (goal.isShared() ? "1" : "0");

        if (goal.isShared()) {
            out << "|";
            auto members = goal.getMembers();
            for (size_t i = 0; i < members.size(); ++i) {
                out << members[i];
                if (i < members.size() - 1) out << ",";
            }
        }
        out << "\n";
    }
}

void StorageManager::loadSavingsFromFile(SavingsManager& savings, const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    int maxId = 0;

    while (getline(in, line)) {
        if (line.empty()) continue;
        auto parts = split(line, '|');
        if (parts[0] != "GOAL" || parts.size() < 9) continue;

        string id = parts[1];
        string name = parts[2];
        double target = stod(parts[3]);
        double current = stod(parts[4]);
        string currency = parts[5];
        string owner = parts[6];
        string deadline = parts[7];
        bool isShared = (parts[8] == "1");

        string numStr = id.substr(1);
        maxId = max(maxId, stoi(numStr));

        SavingsGoal goal(id, name, target, current, currency, owner, deadline, isShared);

        if (isShared && parts.size() > 9) {
            goal.setMembers(split(parts[9], ','));
        }

        savings.addGoal(goal);
    }

    savings.setCounter(maxId + 1);
}