#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include "globals.h"
#include "utils.h"
#include "models.h"
#include "core.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "uk_UA.UTF-8");
    AccountManager manager;
    string dbFilename = "finance_data.txt";

    StorageManager::loadFromFile(manager, dbFilename);

    clearScreen();
    cout << "Оберіть мову / Choose language:\n1. Українська\n2. English\n> ";
    int langChoice;
    while (!(cin >> langChoice) || (langChoice != 1 && langChoice != 2)) {
        cout << "Помилка вводу. 1 або 2: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    AppLanguage lang = (langChoice == 2) ? AppLanguage::English : AppLanguage::Ukrainian;

    string currentUser;
    cin.ignore();
    clearScreen();
    cout << ((lang == AppLanguage::Ukrainian) ? "Увійдіть у систему (Введіть ваше ім'я):\n> " : "Log in (Enter your name):\n> ");
    getline(cin, currentUser);

    while (true) {
        clearScreen();
        cout << "=========================================\n";
        if (lang == AppLanguage::Ukrainian) {
            cout << "Поточний користувач: [" << currentUser << "]\n";
            cout << "-----------------------------------------\n";
            cout << "1. Переглянути баланс моїх рахунків\n";
            cout << "2. Створити новий рахунок\n";
            cout << "3. Поповнити рахунок (Дохід)\n";
            cout << "4. Внести витрату\n";
            cout << "5. Звіт: ТОП-3 моїх витрат\n";
            cout << "6. Змінити користувача (Вийти)\n";
            cout << "7. Змінити мову\n";
            cout << "8. Звіт: Витрати по кожному користувачу\n";
            cout << "0. Вимкнути програму\n";
            cout << "Ваш вибір: ";
        }
        else {
            cout << "Current user: [" << currentUser << "]\n";
            cout << "-----------------------------------------\n";
            cout << "1. View my account balances\n";
            cout << "2. Create new account\n";
            cout << "3. Add income\n";
            cout << "4. Add expense\n";
            cout << "5. Report: TOP-3 my expenses\n";
            cout << "6. Change user (Logout)\n";
            cout << "7. Change language\n";
            cout << "8. Report: Expenses by user\n";
            cout << "0. Exit program\n";
            cout << "Your choice: ";
        }

        int choice;
        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "> ";
        }

        if (choice == 0) {
            StorageManager::saveToFile(manager, dbFilename);
            cout << ((lang == AppLanguage::Ukrainian) ? "До побачення!\n" : "Goodbye!\n");
            break;
        }
        else if (choice == 1) {
            clearScreen();
            cout << ((lang == AppLanguage::Ukrainian) ? "--- ВАШІ РАХУНКИ ---\n" : "--- YOUR ACCOUNTS ---\n");
            auto accounts = manager.getAccounts();
            bool foundAny = false;
            for (const auto& acc : accounts) {
                if (acc->hasAccess(currentUser)) {
                    foundAny = true;
                    cout << "[ID: " << acc->getId() << "] " << acc->getName()
                        << " | Баланс: " << acc->getBalance() << " " << acc->getCurrency();
                    if (acc->isShared()) cout << ((lang == AppLanguage::Ukrainian) ? " (Спільний)" : " (Shared)");
                    cout << "\n";
                }
            }
            if (!foundAny) cout << ((lang == AppLanguage::Ukrainian) ? "У вас немає доступних рахунків.\n" : "You have no accessible accounts.\n");
            waitUser();
        }
        else if (choice == 2) {
            clearScreen();
            int type;
            cout << ((lang == AppLanguage::Ukrainian) ? "Тип (1-Гаманець, 2-Кредитка, 3-Спільний бюджет): " : "Type (1-Wallet, 2-Credit Card, 3-Shared Budget): ");
            while (!(cin >> type) || (type < 1 || type > 3)) {
                cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Введіть 1, 2 або 3: " : "Error! Enter 1, 2 or 3: ");
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            cin.ignore();
            string name, currency;
            cout << ((lang == AppLanguage::Ukrainian) ? "Назва рахунку: " : "Account name: ");
            getline(cin, name);

            while (true) {
                cout << ((lang == AppLanguage::Ukrainian) ? "Валюта (UAH, USD, EUR): " : "Currency (UAH, USD, EUR): ");
                cin >> currency;
                transform(currency.begin(), currency.end(), currency.begin(), ::toupper);
                if (currency == "UAH" || currency == "USD" || currency == "EUR") break;
                else cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Доступні лише UAH, USD, EUR.\n" : "Error! Only UAH, USD, EUR.\n");
            }

            string newId = manager.generateAccId();
            if (type == 2) {
                cout << ((lang == AppLanguage::Ukrainian) ? "Кредитний ліміт: " : "Credit limit: ");
                double limit = getValidDouble();
                manager.addAccount(make_shared<CreditCard>(newId, sanitize(name), currency, sanitize(currentUser), limit));
            }
            else if (type == 3) {
                cin.ignore();
                cout << ((lang == AppLanguage::Ukrainian) ? "Введіть імена ВСІХ учасників через пробіл: " : "Enter ALL member names separated by space: ");
                string usersLine;
                getline(cin, usersLine);
                stringstream ss(usersLine);
                string u;
                vector<string> members;
                while (ss >> u) members.push_back(sanitize(u));
                if (find(members.begin(), members.end(), currentUser) == members.end()) members.push_back(sanitize(currentUser));
                manager.addAccount(make_shared<SharedBudget>(newId, sanitize(name), currency, members));
            }
            else {
                manager.addAccount(make_shared<Wallet>(newId, sanitize(name), currency, sanitize(currentUser)));
            }
            StorageManager::saveToFile(manager, dbFilename);
            cout << ((lang == AppLanguage::Ukrainian) ? "Рахунок створено! Ваш ID: " : "Account created! Your ID: ") << newId << "\n";
            waitUser();
        }
        else if (choice == 3 || choice == 4) {
            clearScreen();
            string accId, category, desc;
            shared_ptr<Account> selectedAcc = nullptr;

            while (true) {
                cout << ((lang == AppLanguage::Ukrainian) ? "Введіть ID рахунку (або '0' для відміни): " : "Enter Account ID (or '0' to cancel): ");
                cin >> accId;
                if (accId == "0") break;

                selectedAcc = manager.getAccountById(accId);
                if (selectedAcc != nullptr) {
                    if (!selectedAcc->hasAccess(currentUser)) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Помилка доступу! Це не ваш рахунок.\n" : "Access denied! Not your account.\n");
                        selectedAcc = nullptr;
                    }
                    else break;
                }
                else cout << ((lang == AppLanguage::Ukrainian) ? "Рахунок не знайдено! Спробуйте ще раз.\n" : "Account not found! Try again.\n");
            }

            if (accId == "0") continue;

            string actingUser = currentUser;
            if (selectedAcc->isShared()) {
                auto sharedAcc = dynamic_pointer_cast<SharedBudget>(selectedAcc);
                auto members = sharedAcc->getMembers();
                cout << ((lang == AppLanguage::Ukrainian) ? "Хто здійснює операцію?\n" : "Who is making the transaction?\n");
                for (size_t i = 0; i < members.size(); ++i) cout << i + 1 << ". " << members[i] << "\n";
                cout << "> ";
                int userChoice;
                while (!(cin >> userChoice) || userChoice < 1 || userChoice > members.size()) {
                    cout << ((lang == AppLanguage::Ukrainian) ? "Невірний вибір: " : "Invalid choice: ");
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                actingUser = members[userChoice - 1];
            }

            cout << ((lang == AppLanguage::Ukrainian) ? "Сума: " : "Amount: ");
            double amount = getValidDouble();

            cin.ignore();
            cout << ((lang == AppLanguage::Ukrainian) ? "Категорія: " : "Category: ");
            getline(cin, category);

            cout << ((lang == AppLanguage::Ukrainian) ? "Опис: " : "Description: ");
            getline(cin, desc);

            cout << ((lang == AppLanguage::Ukrainian) ? "Дата (DD.MM.YYYY): " : "Date (DD.MM.YYYY): ");
            string date = getValidDate(lang);

            bool success = (choice == 3) ? manager.makeIncome(accId, amount, category, desc, date, actingUser)
                : manager.makeExpense(accId, amount, category, desc, date, actingUser);

            if (success) {
                StorageManager::saveToFile(manager, dbFilename);
                cout << ((lang == AppLanguage::Ukrainian) ? "-> Операція успішна!\n" : "-> Operation successful!\n");
            }
            else cout << ((lang == AppLanguage::Ukrainian) ? "-> Помилка! Недостатньо коштів.\n" : "-> Error! Insufficient funds.\n");
            waitUser();
        }
        else if (choice == 5) {
            clearScreen();
            cout << ((lang == AppLanguage::Ukrainian) ? "Початкова дата (DD.MM.YYYY): " : "Start date (DD.MM.YYYY): ");
            string startDate = getValidDate(lang);
            cout << ((lang == AppLanguage::Ukrainian) ? "Кінцева дата (DD.MM.YYYY): " : "End date (DD.MM.YYYY): ");
            string endDate = getValidDate(lang);

            auto topExpenses = ReportGenerator::getTop3Expenses(manager.getTransactionsForUser(currentUser), startDate, endDate);
            cout << "\n=== TOP-3 ===\n";
            if (topExpenses.empty()) cout << ((lang == AppLanguage::Ukrainian) ? "Немає витрат.\n" : "No expenses.\n");
            else {
                for (size_t i = 0; i < topExpenses.size(); ++i) {
                    cout << i + 1 << ". [" << topExpenses[i].userName << "] " << topExpenses[i].category
                        << " (" << topExpenses[i].description << ") - " << topExpenses[i].amount << " | " << topExpenses[i].date << "\n";
                }
            }
            waitUser();
        }
        else if (choice == 6) {
            cin.ignore();
            clearScreen();
            cout << ((lang == AppLanguage::Ukrainian) ? "Увійдіть у систему (Введіть ім'я):\n> " : "Log in:\n> ");
            getline(cin, currentUser);
            cout << ((lang == AppLanguage::Ukrainian) ? "-> Успішно! Ви: " : "-> Success! You are: ") << currentUser << "!\n";
            waitUser();
        }
        else if (choice == 7) {
            clearScreen();
            cout << "Оберіть мову / Choose language:\n1. Українська\n2. English\n> ";
            int newLangChoice;
            while (!(cin >> newLangChoice) || (newLangChoice != 1 && newLangChoice != 2)) {
                cout << "Помилка вводу. 1 або 2: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            lang = (newLangChoice == 2) ? AppLanguage::English : AppLanguage::Ukrainian;
            cout << ((lang == AppLanguage::Ukrainian) ? "-> Мову змінено!\n" : "-> Language changed!\n");
            waitUser();
        }
        else if (choice == 8) {
            clearScreen();
            cout << ((lang == AppLanguage::Ukrainian) ? "Початкова дата (DD.MM.YYYY): " : "Start date (DD.MM.YYYY): ");
            string startDate = getValidDate(lang);
            cout << ((lang == AppLanguage::Ukrainian) ? "Кінцева дата (DD.MM.YYYY): " : "End date (DD.MM.YYYY): ");
            string endDate = getValidDate(lang);

            auto userStats = ReportGenerator::getExpensesByUser(manager.getTransactionsForUser(currentUser), startDate, endDate);
            cout << ((lang == AppLanguage::Ukrainian) ? "\n=== ВИТРАТИ ===\n" : "\n=== EXPENSES ===\n");
            if (userStats.empty()) cout << ((lang == AppLanguage::Ukrainian) ? "Немає витрат.\n" : "No expenses.\n");
            else {
                for (const auto& pair : userStats) cout << "- " << pair.first << ": " << pair.second << "\n";
            }
            waitUser();
        }
    }
    return 0;
}
