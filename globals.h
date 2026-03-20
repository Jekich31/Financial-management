#pragma once
#include <string>

enum class AppLanguage { Ukrainian, English };

struct Transaction {
    std::string id;
    double amount;
    std::string category;
    std::string date;
    std::string description;
    bool isIncome;
    std::string userName;
};
