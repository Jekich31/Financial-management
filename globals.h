#pragma once
#include <string>

enum class AppLanguage { Ukrainian, English };

struct Transaction {
    std::string id;
<<<<<<< HEAD
    double amount;
    std::string category;
    std::string date;
    std::string description;
    bool isIncome;
=======
    double amount = 0.0;        // Добавили начальное значение
    std::string category;
    std::string date;
    std::string description;
    bool isIncome = false;      // Добавили начальное значение
>>>>>>> 5880eb6 (Додав деякі пункти меню, локалізацію, та ін.)
    std::string userName;
};
