#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "types.h"

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
    virtual bool withdraw(double amount) = 0;
    virtual void deposit(double amount) { balance += amount; }
    void addTransaction(const Transaction& t) { history.push_back(t); }

    virtual bool isShared() const { return false; }
    virtual std::string getType() const = 0;
    virtual bool hasAccess(const std::string& userName) const {
        return owner == userName || isShared();
    }
    virtual void setName(const std::string& newName) { name = newName; }
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
    void setLimit(double newLimit) { creditLimit = newLimit; }
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


    void addMember(const std::string& user) {
        if (std::find(members.begin(), members.end(), user) == members.end()) {
            members.push_back(user);
        }
    }

    void removeMember(const std::string& user) {
        members.erase(std::remove(members.begin(), members.end(), user), members.end());
    }

};

class SavingsGoal {
private:
    std::string id;
    std::string name;
    double targetAmount;
    double currentAmount;
    std::string currency;
    std::string owner;
    std::vector<std::string> members;
    std::string deadline;
    bool shared;
public:
    SavingsGoal(std::string id, std::string name, double target, double current,
                std::string currency, std::string owner, std::string deadline, bool isShared)
        : id(id), name(name), targetAmount(target), currentAmount(current),
          currency(currency), owner(owner), deadline(deadline), shared(isShared) {}

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    double getTargetAmount() const { return targetAmount; }
    double getCurrentAmount() const { return currentAmount; }
    std::string getCurrency() const { return currency; }
    std::string getOwner() const { return owner; }
    std::string getDeadline() const { return deadline; }
    bool isShared() const { return shared; }
    std::vector<std::string> getMembers() const { return members; }

    void setName(const std::string& n) { name = n; }
    void setTargetAmount(double t) { targetAmount = t; }
    void setDeadline(const std::string& d) { deadline = d; }
    void setMembers(const std::vector<std::string>& m) { members = m; }

    void addFunds(double amount) { currentAmount += amount; }
    bool withdrawFunds(double amount) {
        if (currentAmount >= amount) { currentAmount -= amount; return true; }
        return false;
    }

    double getProgress() const {
        return (targetAmount > 0) ? (currentAmount / targetAmount * 100.0) : 0.0;
    }

    void addMember(const std::string& user) {
        if (std::find(members.begin(), members.end(), user) == members.end())
            members.push_back(user);
    }
    void removeMember(const std::string& user) {
        members.erase(std::remove(members.begin(), members.end(), user), members.end());
    }

    bool hasAccess(const std::string& userName) const {
        if (!shared) return owner == userName;
        return std::find(members.begin(), members.end(), userName) != members.end();
    }
};
