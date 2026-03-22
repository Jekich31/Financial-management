#pragma once
#include <string>

enum class AppLanguage { Ukrainian, English };

struct Transaction {
    std::string id;
    double amount = 0.0;        // Добавили начальное значение
    std::string category;
    std::string date;
    std::string description;
    bool isIncome = false;      // Добавили начальное значение
    std::string userName;
};
