#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "globals.h"

class Account {
protected:
    std::string id;
    std::string name;
    double balance;
    std::string currency;
    std::string owner;
    std::vector<Transaction> history;
public:
    Account(std::string id, std::string name, std::string curr, std::string ownerName, double initialBalance = 0.0)
        : id(id), name(name), balance(initialBalance), currency(curr), owner(ownerName) {
    }
    virtual ~Account() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    double getBalance() const { return balance; }
    std::string getCurrency() const { return currency; }
    std::string getOwner() const { return owner; }
    std::vector<Transaction> getHistory() const { return history; }
<<<<<<< HEAD

=======
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
    virtual bool withdraw(double amount) = 0;
    virtual void deposit(double amount) { balance += amount; }
    void addTransaction(const Transaction& t) { history.push_back(t); }

    virtual bool isShared() const { return false; }
    virtual std::string getType() const = 0;
    virtual bool hasAccess(const std::string& userName) const {
        return owner == userName || isShared();
    }
<<<<<<< HEAD
=======
    virtual void setName(const std::string& newName) { name = newName; }
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
};

class Wallet : public Account {
public:
    Wallet(std::string id, std::string name, std::string curr, std::string ownerName, double initialBalance = 0.0)
        : Account(id, name, curr, ownerName, initialBalance) {
    }
    bool withdraw(double amount) override {
        if (balance >= amount) { balance -= amount; return true; }
        return false;
    }
    std::string getType() const override { return "WALLET"; }
};

class CreditCard : public Account {
private:
    double creditLimit;
public:
    CreditCard(std::string id, std::string name, std::string curr, std::string ownerName, double limit, double initialBalance = 0.0)
        : Account(id, name, curr, ownerName, initialBalance), creditLimit(limit) {
    }
    bool withdraw(double amount) override {
        if (balance + creditLimit >= amount) { balance -= amount; return true; }
        return false;
    }
    std::string getType() const override { return "CREDIT_CARD"; }
    double getLimit() const { return creditLimit; }
<<<<<<< HEAD
=======
    void setLimit(double newLimit) { creditLimit = newLimit; }
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
};

class SharedBudget : public Account {
private:
    std::vector<std::string> members;
public:
    SharedBudget(std::string id, std::string name, std::string curr, std::vector<std::string> users, double initialBalance = 0.0)
        : Account(id, name, curr, "Shared", initialBalance), members(users) {
    }

    bool withdraw(double amount) override {
        if (balance >= amount) { balance -= amount; return true; }
        return false;
    }
    bool isShared() const override { return true; }
    std::string getType() const override { return "SHARED_BUDGET"; }
    std::vector<std::string> getMembers() const { return members; }
    bool hasAccess(const std::string& userName) const override {
        return std::find(members.begin(), members.end(), userName) != members.end();
    }
<<<<<<< HEAD
=======


    void addMember(const std::string& user) {
        if (std::find(members.begin(), members.end(), user) == members.end()) {
            members.push_back(user);
        }
    }

    void removeMember(const std::string& user) {
        members.erase(std::remove(members.begin(), members.end(), user), members.end());
    }

>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
};
