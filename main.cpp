#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <limits>
#include <vector>
#include "globals.h"
#include "utils.h"
#include "models.h"
#include "core.h"
#include "test.h"
using namespace std;

// Допоміжна функція для швидкого показу рахунків
void showFastAccountList(AccountManager& manager, const string& currentUser, AppLanguage lang) {
    auto accounts = manager.getAccounts();
    cout << "-----------------------------------------\n";
    if (lang == AppLanguage::Ukrainian) cout << "Доступні рахунки (ID | Назва | Баланс):\n";
    else cout << "Available accounts (ID | Name | Balance):\n";

    bool found = false;
    for (const auto& acc : accounts) {
        if (acc->hasAccess(currentUser)) {
            cout << " > [" << acc->getId() << "] " << acc->getName()
                << " (" << acc->getBalance() << " " << acc->getCurrency() << ")";
            if (acc->isShared()) cout << ((lang == AppLanguage::Ukrainian) ? " (Спільний)" : " (Shared)");
            cout << "\n";
            found = true;
        }
    }
    if (!found) {
        if (lang == AppLanguage::Ukrainian) cout << "Рахунків не знайдено.\n";
        else cout << "No accounts found.\n";
    }
    cout << "-----------------------------------------\n";
}

string toSortable(const string& date) {
    if (date.length() < 10) return "0"; // Якщо це "0" або некоректна дата

    string day = date.substr(0, 2);
    string month = date.substr(3, 2);
    string year = date.substr(6, 4);

    return year + month + day;
}



string getValidEndDate(const string& startDate, AppLanguage lang) {
    string endDate;
    while (true) {
        cout << ((lang == AppLanguage::Ukrainian) ? "Кінцева дата (0-відміна): " : "End date (0-cancel): ");
        endDate = getValidDate(lang);

        if (endDate == "0") return "0";

        // Порівнюємо перетворені дати
        if (toSortable(endDate) >= toSortable(startDate)) {
            return endDate;
        }

        // Вивід помилки, якщо дата "в минулому" відносно початкової
        cout << ((lang == AppLanguage::Ukrainian)
            ? "Помилка: Кінцева дата не може бути раніше початкової!"
            : "Error: End date cannot be before start date!") << endl;
    }
}



int main() {
    system("chcp 65001 > nul");
    setlocale(LC_ALL, "uk_UA.UTF-8");
    AccountManager manager;
    string dbFilename = "finance_data.txt";

    StorageManager::loadFromFile(manager, dbFilename);

    clearScreen();
    cout << "Оберіть мову / Choose language:\n1. Українська\n2. English\n> ";
    vector<string> langOptions = { "Українська", "English" };
    int langIdx = interactiveMenu("Оберіть мову / Choose language:", langOptions);
    AppLanguage lang = (langIdx == 1) ? AppLanguage::English : AppLanguage::Ukrainian;

    string currentUser;
    clearScreen();
    cout << ((lang == AppLanguage::Ukrainian) ? "Увійдіть у систему (Введіть ваше ім'я):\n> " : "Log in (Enter your name):\n> ");
    getline(cin, currentUser);

    // ==========================================
    // ГОЛОВНИЙ ЦИКЛ ПРОГРАМИ
    // ==========================================
    while (true) {
        string header = (lang == AppLanguage::Ukrainian)
            ? "Поточний користувач: [" + currentUser + "]"
            : "Current user: [" + currentUser + "]";

        vector<string> menuOptions;
        if (lang == AppLanguage::Ukrainian) {
            menuOptions = {
                "💼 Рахунки (Перегляд, Створення, Редагування...)",
                "💸 Транзакції (Доходи, Витрати, Перекази)",
                "📊 Історія та Звіти (Виписка, ТОП-3 витрат...)",
                "🌍 Капітал та Курси валют",
                "⚙️ Налаштування (Змінити мову / користувача)",
                "❌ Вимкнути програму"
            };
        }
        else {
            menuOptions = {
                "💼 Accounts (View, Create, Edit...)",
                "💸 Transactions (Income, Expenses, Transfers)",
                "📊 History & Reports (Statement, TOP-3...)",
                "🌍 Total Net Worth & Exchange Rates",
                "⚙️ Settings (Change language / user)",
                "❌ Exit program"
            };
        }
        int selectedIndex = interactiveMenu(header, menuOptions);

        int mainChoice;
        if (selectedIndex == 5) {
            mainChoice = 0;
        }
        else {
            mainChoice = selectedIndex + 1;
        }

        if (mainChoice == 0) {
            StorageManager::saveToFile(manager, dbFilename);
            cout << ((lang == AppLanguage::Ukrainian) ? "До побачення!\n" : "Goodbye!\n");
            break; // Вихід з програми
        }

        // ==========================================
        // ПІДМЕНЮ 1: РАХУНКИ
        // ==========================================
        else if (mainChoice == 1) {
            while (true) {
                string header = (lang == AppLanguage::Ukrainian)
                    ? "--- 💼 УПРАВЛІННЯ РАХУНКАМИ ---"
                    : "--- 💼 ACCOUNT MANAGEMENT ---";

                vector<string> options;
                if (lang == AppLanguage::Ukrainian) {
                    options = {
                        "1. Переглянути мої рахунки",
                        "2. Створити новий рахунок",
                        "3. Редагувати рахунок",
                        "4. Видалити рахунок",
                        "0. <-- Назад до Головного меню"
                    };
                }
                else {
                    options = {
                        "1. View my accounts",
                        "2. Create new account",
                        "3. Edit account",
                        "4. Delete account",
                        "0. <-- Back to Main Menu"
                    };
                }

                int sel = interactiveMenu(header, options);

                // Переводимо індекс меню (0..4) у ваш старий формат sub
                int sub;
                if (sel == 4) sub = 0; // Останній пункт - це "Назад" (0)
                else sub = sel + 1;
                if (sub == 0) break; // Повернення в головне меню

                if (sub == 1) { // Перегляд балансу
                    clearScreen();
                    cout << ((lang == AppLanguage::Ukrainian) ? "--- ВАШІ РАХУНКИ ---\n" : "--- YOUR ACCOUNTS ---\n");
                    showFastAccountList(manager, currentUser, lang);
                    waitUser();
                }
                else if (sub == 2) { // Створення рахунку
                    clearScreen();
                    int type;
                    cout << ((lang == AppLanguage::Ukrainian) ? "Тип (0-Відміна, 1-Скарбничка, 2-Кредитка, 3-Спільний бюджет): " : "Type (0-Cancel, 1-Debit wallet, 2-Credit Card, 3-Shared Budget): ");
                    while (!(cin >> type) || (type < 0 || type > 3)) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Введіть 0, 1, 2 або 3: " : "Error! Enter 0, 1, 2 or 3: ");
                        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    if (type == 0) continue;

                    cin.ignore();
                    string name, currency;
                    cout << ((lang == AppLanguage::Ukrainian) ? "Назва рахунку: " : "Account name: ");
                    getline(cin, name);

                    int currChoice;
                    cout << ((lang == AppLanguage::Ukrainian) ? "Оберіть валюту (1-UAH, 2-USD, 3-EUR): " : "Choose currency (1-UAH, 2-USD, 3-EUR): ");
                    while (!(cin >> currChoice) || currChoice < 1 || currChoice > 3) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Введіть 1, 2 або 3: " : "Error! Enter 1, 2 or 3: ");
                        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    if (currChoice == 1) currency = "UAH";
                    else if (currChoice == 2) currency = "USD";
                    else currency = "EUR";

                    string newId = manager.generateAccId();

                    if (type == 2) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Кредитний ліміт: " : "Credit limit: ");
                        double limit = getValidDouble();
                        cout << ((lang == AppLanguage::Ukrainian) ? "Власні кошти (початковий баланс): " : "Own funds (initial balance): ");
                        double initialBalance = getValidDouble();

                        auto newCard = make_shared<CreditCard>(newId, sanitize(name), currency, sanitize(currentUser), limit);
                        if (initialBalance > 0) newCard->deposit(initialBalance);
                        manager.addAccount(newCard);
                    }
                    else if (type == 3) {
                        cin.ignore();
                        cout << ((lang == AppLanguage::Ukrainian) ? "Введіть імена ВСІХ учасників через пробіл: " : "Enter ALL member names separated by space: ");
                        string usersLine; getline(cin, usersLine);
                        stringstream ss(usersLine); string u; vector<string> members;
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
                else if (sub == 3) { // Редагування
                    clearScreen(); showFastAccountList(manager, currentUser, lang);
                    cout << ((lang == AppLanguage::Ukrainian) ? "ID рахунку для зміни (0-відміна): " : "Account ID to edit (0-cancel): ");
                    string accId; cin >> accId;
                    if (accId == "0") continue;

                    auto acc = manager.getAccountById(accId);
                    if (acc && acc->getOwner() == currentUser) {
                        cin.ignore();
                        cout << ((lang == AppLanguage::Ukrainian) ? "Нова назва рахунку: " : "New account name: ");
                        string newName; getline(cin, newName);
                        double newLimit = -1.0;
                        if (acc->getType() == "CREDIT_CARD") {
                            cout << ((lang == AppLanguage::Ukrainian) ? "Новий кредитний ліміт: " : "New credit limit: ");
                            newLimit = getValidDouble();
                        }
                        if (manager.updateAccount(accId, currentUser, newName, newLimit)) {
                            StorageManager::saveToFile(manager, dbFilename);
                        }
                    }
                    else {
                        cout << ((lang == AppLanguage::Ukrainian) ? "-> Помилка доступу!\n" : "-> Access denied!\n");
                    }
                    waitUser();
                }
                else if (sub == 4) { // Видалення
                    clearScreen(); showFastAccountList(manager, currentUser, lang);
                    cout << ((lang == AppLanguage::Ukrainian) ? "ID рахунку для видалення (0-відміна): " : "Account ID to delete (0-cancel): ");
                    string accId; cin >> accId;
                    if (accId != "0" && manager.deleteAccount(accId, currentUser)) {
                        StorageManager::saveToFile(manager, dbFilename);
                    }
                    else if (accId == "0") {
                        continue;
                    }
                    waitUser();
                }
            }
        }

        // ==========================================
        // ПІДМЕНЮ 2: ТРАНЗАКЦІЇ
        // ==========================================
        else if (mainChoice == 2) {
            while (true) {
                string header = (lang == AppLanguage::Ukrainian)
                    ? "--- 💸 ТРАНЗАКЦІЇ ---"
                    : "--- 💸 TRANSACTIONS ---";

                vector<string> options;
                if (lang == AppLanguage::Ukrainian) {
                    options = {
                        "1. Поповнити рахунок (Дохід)",
                        "2. Внести витрату",
                        "3. Переказ між рахунками",
                        "0. <-- Назад до Головного меню"
                    };
                }
                else {
                    options = {
                        "1. Add income",
                        "2. Add expense",
                        "3. Transfer funds",
                        "0. <-- Back to Main Menu"
                    };
                }

                int sel = interactiveMenu(header, options);

                int sub;
                if (sel == 3) sub = 0; // Останній пункт (індекс 3) - це "Назад"
                else sub = sel + 1;
                if (sub == 0) break;

                if (sub == 1 || sub == 2) { // Дохід або Витрата
                    clearScreen(); showFastAccountList(manager, currentUser, lang);
                    string accId, category, desc;
                    shared_ptr<Account> selectedAcc = nullptr;

                    cout << ((lang == AppLanguage::Ukrainian) ? "Введіть ID рахунку (0 для відміни): " : "Enter Account ID (0 to cancel): ");
                    cin >> accId;
                    if (accId == "0") continue;

                    selectedAcc = manager.getAccountById(accId);
                    if (!selectedAcc || !selectedAcc->hasAccess(currentUser)) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Помилка доступу!\n" : "Access denied!\n");
                        waitUser(); continue;
                    }

                    string actingUser = currentUser;
                    if (selectedAcc->isShared()) {
                        auto sharedAcc = dynamic_pointer_cast<SharedBudget>(selectedAcc);
                        auto members = sharedAcc->getMembers();
                        cout << ((lang == AppLanguage::Ukrainian) ? "Хто здійснює операцію?\n" : "Who is making the transaction?\n");
                        for (size_t i = 0; i < members.size(); ++i) cout << i + 1 << ". " << members[i] << "\n";
                        cout << "> ";
                        int userChoice;
                        while (!(cin >> userChoice) || userChoice < 1 || userChoice > members.size()) {
                            cout << "Error: "; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

                    bool success = (sub == 1) ? manager.makeIncome(accId, amount, category, desc, date, actingUser)
                        : manager.makeExpense(accId, amount, category, desc, date, actingUser);
                    if (success) {
                        StorageManager::saveToFile(manager, dbFilename);
                        cout << ((lang == AppLanguage::Ukrainian) ? "-> Операція успішна!\n" : "-> Operation successful!\n");
                    }
                    else cout << ((lang == AppLanguage::Ukrainian) ? "-> Помилка! Недостатньо коштів.\n" : "-> Error! Insufficient funds.\n");
                    waitUser();
                }
                else if (sub == 3) { // Переказ
                    clearScreen(); showFastAccountList(manager, currentUser, lang);
                    cout << ((lang == AppLanguage::Ukrainian) ? "ID ВІДПРАВНИКА (0-відміна): " : "SENDER ID (0-cancel): ");
                    string fromId; cin >> fromId; if (fromId == "0") continue;

                    cout << ((lang == AppLanguage::Ukrainian) ? "ID ОТРИМУВАЧА: " : "RECEIVER ID: ");
                    string toId; cin >> toId;

                    auto accFrom = manager.getAccountById(fromId);
                    auto accTo = manager.getAccountById(toId);
                    if (!accFrom || !accTo) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Рахунок не знайдено.\n" : "Error! Account not found.\n");
                        waitUser(); continue;
                    }

                    if (accFrom->getCurrency() != accTo->getCurrency()) {
                        double rateFrom = CurrencyManager::getInstance().getRate(accFrom->getCurrency());
                        double rateTo = CurrencyManager::getInstance().getRate(accTo->getCurrency());
                        cout << "\n[!] " << ((lang == AppLanguage::Ukrainian) ? "Курс переказу: " : "Transfer rate: ")
                            << "1 " << accFrom->getCurrency() << " = " << (rateFrom / rateTo) << " " << accTo->getCurrency() << "\n\n";
                    }

                    cout << ((lang == AppLanguage::Ukrainian) ? "Сума переказу: " : "Transfer amount: ");
                    double amount = getValidDouble();
                    cout << ((lang == AppLanguage::Ukrainian) ? "Дата (DD.MM.YYYY): " : "Date (DD.MM.YYYY): ");
                    string date = getValidDate(lang);

                    if (manager.transferFunds(fromId, toId, amount, date, currentUser)) {
                        StorageManager::saveToFile(manager, dbFilename);
                        cout << ((lang == AppLanguage::Ukrainian) ? "-> Переказ виконано!\n" : "-> Transfer successful!\n");
                    }
                    else {
                        cout << ((lang == AppLanguage::Ukrainian) ? "-> Помилка переказу.\n" : "-> Transfer error.\n");
                    }
                    waitUser();
                }
            }
        }

        // ==========================================
        // ПІДМЕНЮ 3: ІСТОРІЯ ТА ЗВІТИ
        // ==========================================
        else if (mainChoice == 3) {
            while (true) {
                string header = (lang == AppLanguage::Ukrainian) ? "--- 📊 ІСТОРІЯ ТА ЗВІТИ ---" : "--- 📊 HISTORY & REPORTS ---";
                vector<string> options;

                if (lang == AppLanguage::Ukrainian) {
                    options = {
                        "1. Виписка по рахунку (Історія транзакцій)",
                        "2. Звіт: ТОП-3 моїх витрат",
                        "3. Звіт: Витрати по користувачах",
                        "4. Загальний фінансовий звіт",
                        "5. Звіт: ТОП-3 категорій витрат",
                        "0. <-- Назад до Головного меню"
                    };
                }
                else {
                    options = {
                        "1. Account Statement",
                        "2. Report: TOP-3 expenses",
                        "3. Report: Expenses by user",
                        "4. General Financial Report",
                        "5. Report: TOP-3 expense categories",
                        "0. <-- Back to Main Menu"
                    };
                }

                int sel = interactiveMenu(header, options);
                int sub = (sel == 5) ? 0 : sel + 1;
                if (sub == 0) break;

                if (sub == 1) {
                    clearScreen(); showFastAccountList(manager, currentUser, lang);
                    cout << ((lang == AppLanguage::Ukrainian) ? "Введіть ID рахунку (0-відміна): " : "Enter Account ID (0-cancel): ");
                    string accId; cin >> accId;
                    if (accId == "0") continue;
                    if (accId != "0") {
                        auto acc = manager.getAccountById(accId);
                        if (!acc || !acc->hasAccess(currentUser)) {
                            cout << ((lang == AppLanguage::Ukrainian) ? "Помилка доступу.\n" : "Access denied.\n");
                        }
                        else {
                            cout << "\n=== " << ((lang == AppLanguage::Ukrainian) ? "ІСТОРІЯ: " : "HISTORY: ") << acc->getName() << " ===\n";
                            auto hist = acc->getHistory();
                            if (hist.empty()) cout << ((lang == AppLanguage::Ukrainian) ? "Порожньо.\n" : "Empty.\n");
                            else {
                                for (const auto& tx : hist) {
                                    string sym = tx.isIncome ? "+" : "-";
                                    if (tx.category == "Transfer" || tx.category == "Transfer In" || tx.category == "Transfer Out") sym = "<->";
                                    cout << "[" << tx.date << "] " << sym << " " << tx.amount << " " << acc->getCurrency()
                                        << " | " << tx.category << " | " << tx.description << " (" << tx.userName << ")\n";
                                }
                            }
                        }
                    }
                    waitUser();
                }
                else if (sub == 2 || sub == 3) {
                    clearScreen();
                    cout << ((lang == AppLanguage::Ukrainian) ? "Початкова дата (0-відміна): " : "Start date (0-cancel): ");
                    string startDate = getValidDate(lang);
                    if (startDate == "0") continue;
                    string endDate = getValidEndDate(startDate, lang);
                    if (endDate == "0") continue;
                    cout << ((lang == AppLanguage::Ukrainian) ? "Дати прийняті: " : "Dates accepted: ")
                        << startDate << " - " << endDate << endl;

                    if (sub == 2) {
                        auto top = ReportGenerator::getTop3Expenses(manager.getTransactionsForUser(currentUser), startDate, endDate);
                        cout << "\n=== TOP-3 ===\n";
                        if (top.empty()) cout << ((lang == AppLanguage::Ukrainian) ? "Немає витрат.\n" : "No expenses.\n");
                        for (size_t i = 0; i < top.size(); ++i) {
                            cout << i + 1 << ". [" << top[i].userName << "] " << top[i].category << " - " << top[i].amount << " | " << top[i].date << "\n";
                        }
                    }
                    else {
                        auto stats = ReportGenerator::getExpensesByUser(manager.getTransactionsForUser(currentUser), startDate, endDate);
                        cout << "\n=== " << ((lang == AppLanguage::Ukrainian) ? "ВИТРАТИ" : "EXPENSES") << " ===\n";
                        if (stats.empty()) cout << ((lang == AppLanguage::Ukrainian) ? "Немає витрат.\n" : "No expenses.\n");
                        for (const auto& pair : stats) cout << "- " << pair.first << ": " << pair.second << "\n";
                    }
                    waitUser();
                }
                else if (sub == 4) {
                    clearScreen();
                    cout << ((lang == AppLanguage::Ukrainian) ? "Початкова дата (0-відміна): " : "Start date (0-cancel): ");
                    string startDate = getValidDate(lang);
                    if (startDate == "0") continue;
                    string endDate = getValidEndDate(startDate, lang);
                    if (endDate == "0") continue;
                    cout << ((lang == AppLanguage::Ukrainian) ? "Дати прийняті: " : "Dates accepted: ")
                        << startDate << " - " << endDate << endl;

                    cout << "\n=== " << ((lang == AppLanguage::Ukrainian) ? "ЗАГАЛЬНИЙ ЗВІТ" : "GENERAL REPORT") << " ===\n";
                    cout << ((lang == AppLanguage::Ukrainian) ? "Період: " : "Period: ") << startDate << " - " << endDate << "\n\n";

                    bool found = false;
                    for (const auto& acc : manager.getAccounts()) {
                        if (acc->hasAccess(currentUser)) {
                            found = true;
                            double accInc = 0, accExp = 0, accTrIn = 0, accTrOut = 0;

                            for (const auto& tx : acc->getHistory()) {
                                if (tx.date >= startDate && tx.date <= endDate) {
                                    if (tx.category == "Transfer" || tx.category == "Transfer In" || tx.category == "Transfer Out") {
                                        if (tx.isIncome) accTrIn += tx.amount;
                                        else accTrOut += tx.amount;
                                    }
                                    else if (tx.isIncome) {
                                        accInc += tx.amount;
                                    }
                                    else {
                                        accExp += tx.amount;
                                    }
                                }
                            }

                            cout << "[" << acc->getName() << "] (" << acc->getCurrency() << ")\n";
                            cout << "  + " << ((lang == AppLanguage::Ukrainian) ? "Доходи: " : "Income: ") << accInc << "\n";
                            cout << "  - " << ((lang == AppLanguage::Ukrainian) ? "Витрати: " : "Expenses: ") << accExp << "\n";
                            cout << "  <-> " << ((lang == AppLanguage::Ukrainian) ? "Вхідні перекази: " : "Incoming transfers: ") << accTrIn << "\n";
                            cout << "  <-> " << ((lang == AppLanguage::Ukrainian) ? "Вихідні перекази: " : "Outgoing transfers: ") << accTrOut << "\n\n";
                        }
                    }
                    if (!found) cout << ((lang == AppLanguage::Ukrainian) ? "Рахунків не знайдено.\n" : "No accounts found.\n");
                    waitUser();
                }
                else if (sub == 5) {
                    clearScreen();

                    cout << ((lang == AppLanguage::Ukrainian) ? "Початкова дата (0-відміна): " : "Start date (0-cancel): ");
                    string startDate = getValidDate(lang);
                    if (startDate == "0") continue;

                    string endDate = getValidEndDate(startDate, lang);
                    if (endDate == "0") continue;

                    auto top = ReportGenerator::getTop3Categories(
                        manager.getTransactionsForUser(currentUser),
                        startDate,
                        endDate
                    );

                    cout << "\n=== TOP-3 "
                        << ((lang == AppLanguage::Ukrainian) ? "КАТЕГОРІЙ" : "CATEGORIES")
                        << " ===\n";

                    if (top.empty()) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Немає витрат.\n" : "No expenses.\n");
                    }
                    else {
                        for (size_t i = 0; i < top.size(); ++i) {
                            cout << i + 1 << ". " << top[i].first
                                << " - " << top[i].second << "\n";
                        }
                    }

                    waitUser();
                }
            }
        }

        // ==========================================
        // ПІДМЕНЮ 4: КАПІТАЛ ТА ВАЛЮТИ
        // ==========================================
        else if (mainChoice == 4) {
            while (true) {
                string header = (lang == AppLanguage::Ukrainian) ? "--- 🌍 КАПІТАЛ ТА ВАЛЮТИ ---" : "--- 🌍 TOTAL NET WORTH & CURRENCIES ---";
                vector<string> options;

                if (lang == AppLanguage::Ukrainian) {
                    options = { "1. Розрахувати мій Загальний капітал", "2. Оновити курси валют", "0. <-- Назад" };
                }
                else {
                    options = { "1. Calculate Total Net Worth", "2. Update exchange rates", "0. <-- Back" };
                }

                int sel = interactiveMenu(header, options);
                int sub = (sel == 2) ? 0 : sel + 1;
                if (sub == 0) break;

                if (sub == 1) { // Капітал
                    clearScreen();
                    int targetCurr;
                    cout << ((lang == AppLanguage::Ukrainian) ? "Оберіть валюту (0-Вихід, 1-UAH, 2-USD, 3-EUR): " : "Choose currency (0-exit, 1-UAH, 2-USD, 3-EUR): ");
                    while (!(cin >> targetCurr) || targetCurr < 0 || targetCurr > 3) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Введіть 1, 2 або 3: " : "Error! Enter 1, 2 or 3: ");
                        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    if (targetCurr == 0) continue;
                    std::string currencyStr;
                    switch (targetCurr) {
                    case 1: currencyStr = "UAH"; break;
                    case 2: currencyStr = "USD"; break;
                    case 3: currencyStr = "EUR"; break;
                    default: currencyStr = "UNKNOWN"; break;
                    }
                    double total = 0.0; bool hasAcc = false;
                    try {
                        for (const auto& acc : manager.getAccounts()) {
                            if (acc->hasAccess(currentUser)) {
                                hasAcc = true;
                                total += CurrencyManager::getInstance().convert(acc->getBalance(), acc->getCurrency(), currencyStr);
                            }
                        }
                        if (!hasAcc) cout << ((lang == AppLanguage::Ukrainian) ? "Рахунків немає.\n" : "No accounts.\n");
                        else cout << "\n" << ((lang == AppLanguage::Ukrainian) ? "Загальний баланс: " : "Total balance: ") << total << " " << currencyStr << "\n";
                    }
                    catch (...) {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Валюта не підтримується.\n" : "Error! Currency not supported.\n");
                    }
                    waitUser();
                }
                else if (sub == 2) { // Оновити курси
                    clearScreen();
                    cout << ((lang == AppLanguage::Ukrainian) ? "Поточні курси (до UAH):\n" : "Current rates (to UAH):\n");
                    cout << "USD: " << CurrencyManager::getInstance().getRate("USD") << "\n";
                    cout << "EUR: " << CurrencyManager::getInstance().getRate("EUR") << "\n\n";

                    string curr;
                    cout << ((lang == AppLanguage::Ukrainian) ? "Яку валюту оновити? (USD, EUR або 0): " : "Currency to update? (USD, EUR or 0): ");
                    cin >> curr; if (curr == "0") continue;
                    transform(curr.begin(), curr.end(), curr.begin(), ::toupper);

                    if (curr == "USD" || curr == "EUR") {
                        cout << ((lang == AppLanguage::Ukrainian) ? "Новий курс: " : "New rate: ");
                        double newRate = getValidDouble();
                        CurrencyManager::getInstance().updateRate(curr, newRate);
                        cout << "-> OK!\n";
                    }
                    else cout << "Error.\n";
                    waitUser();
                }
            }
        }

        // ==========================================
        // ПІДМЕНЮ 5: НАЛАШТУВАННЯ
        // ==========================================
        else if (mainChoice == 5) {
            while (true) {
                string header = (lang == AppLanguage::Ukrainian) ? "--- ⚙️ НАЛАШТУВАННЯ ---" : "--- ⚙️ SETTINGS ---";
                vector<string> options;

                if (lang == AppLanguage::Ukrainian) {
                    options = { "1. Змінити користувача", "2. Змінити мову інтерфейсу", "0. <-- Назад" };
                }
                else {
                    options = { "1. Change user", "2. Change interface language", "0. <-- Back" };
                }

                int sel = interactiveMenu(header, options);
                int sub = (sel == 2) ? 0 : sel + 1;

                if (sub == 0) break;

                if (sub == 1) { // Змінити користувача
                    clearScreen();
                    cout << ((lang == AppLanguage::Ukrainian) ? "Введіть ім'я (0-вихід):\n> " : "Enter name (0-exit):\n> ");
                    getline(cin, currentUser);
                    if (currentUser == "0") continue;
                    cout << "-> OK: " << currentUser << "!\n";
                    waitUser(); break; // Виходимо в головне меню після зміни
                }
                else if (sub == 2) { // Змінити мову
                    string langHeader = (lang == AppLanguage::Ukrainian)
                        ? "--- ОБЕРІТЬ МОВУ ---"
                        : "--- CHOOSE LANGUAGE ---";

                    vector<string> langOptions;
                    if (lang == AppLanguage::Ukrainian) {
                        langOptions = { "1. Українська", "2. English", "0. Скасувати" };
                    }
                    else {
                        langOptions = { "1. Ukrainian", "2. English", "0. Cancel" };
                    }

                    // Викликаємо наше інтерактивне меню
                    int langIdx = interactiveMenu(langHeader, langOptions);

                    // Обробка вибору:
                    // langIdx 0 -> Українська
                    // langIdx 1 -> English
                    // langIdx 2 -> Скасувати (0)

                    if (langIdx == 0) {
                        lang = AppLanguage::Ukrainian;
                        cout << (lang == AppLanguage::Ukrainian ? "-> Мову змінено!\n" : "-> Language changed!\n");
                        waitUser();
                    }
                    else if (langIdx == 1) {
                        lang = AppLanguage::English;
                        cout << (lang == AppLanguage::Ukrainian ? "-> Мову змінено!\n" : "-> Language changed!\n");
                        waitUser();
                    }
                    else if (langIdx == 2) {
                        // Користувач вибрав "Скасувати", просто повертаємось назад
                        continue;
                    }
                }
            }
        }
    }
    return 0;
}
