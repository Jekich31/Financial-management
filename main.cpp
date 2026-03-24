#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <limits>
#include "globals.h"
#include "utils.h"
#include "models.h"
#include "core.h"

using namespace std;
void showFastAccountList(AccountManager& manager, const string& currentUser, AppLanguage lang) {
	auto accounts = manager.getAccounts();
	cout << "-----------------------------------------\n";
	if (lang == AppLanguage::Ukrainian) cout << "Доступні рахунки (ID | Назва | Баланс):\n";
	else cout << "Available accounts (ID | Name | Balance):\n";

	bool found = false;
	for (const auto& acc : accounts) {
		if (acc->hasAccess(currentUser)) {
			cout << " > [" << acc->getId() << "] " << acc->getName()
				<< " (" << acc->getBalance() << " " << acc->getCurrency() << ")\n";
			found = true;
		}
	}
	if (!found) {
		if (lang == AppLanguage::Ukrainian) cout << "Рахунків не знайдено.\n";
		else cout << "No accounts found.\n";
	}
	cout << "-----------------------------------------\n";
}
int main() {
	system("chcp 65001 > nul");
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

	// ПОЧАТОК ГОЛОВНОГО ЦИКЛУ
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
			cout << "9. Видалити рахунок\n";
			cout << "10. Редагувати рахунок\n";
			cout << "11. Переказ коштів між рахунками\n";
			cout << "12. Загальний капітал (Конвертер валют)\n";
			cout << "13. Оновити курси валют\n";
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
			cout << "9. Delete account\n";
			cout << "10. Edit account\n";
			cout << "11. Transfer funds between accounts\n";
			cout << "12. Total net worth (Currency Converter)\n";
            cout << "13. Update exchange rates\n";
			cout << "0. Exit program\n";
			cout << "Your choice: ";
		}

		int choice;
		if (!(cin >> choice)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}

		if (choice == 0) {
			StorageManager::saveToFile(manager, dbFilename);
			cout << ((lang == AppLanguage::Ukrainian) ? "До побачення!\n" : "Goodbye!\n");
			break; // Вихід з циклу
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
			cout << ((lang == AppLanguage::Ukrainian) ? "Тип (0-Вихід, 1-Гаманець, 2-Кредитка, 3-Спільний бюджет): " : "Type (0-Exit, 1-Wallet, 2-Credit Card, 3-Shared Budget): ");
			while (!(cin >> type) || (type < 0 || type > 3)) {
				cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Введіть 0, 1, 2 або 3: " : "Error! Enter 1, 2 or 3: ");
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			if (type == 0) continue;
			cin.ignore();
			string name, currency;
			cout << ((lang == AppLanguage::Ukrainian) ? "Назва рахунку: " : "Account name: ");
			getline(cin, name);
			if (type == 0) continue;
            
            cin.ignore();
            
            // --- ПОЧАТОК НОВОГО КОДУ ---
            int currChoice;
            cout << ((lang == AppLanguage::Ukrainian) ? "Оберіть валюту (1-UAH, 2-USD, 3-EUR): " : "Choose currency (1-UAH, 2-USD, 3-EUR): ");
            while (!(cin >> currChoice) || currChoice < 1 || currChoice > 3) {
                cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Введіть 1, 2 або 3: " : "Error! Enter 1, 2 or 3: ");
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            if (currChoice == 1) currency = "UAH";
            else if (currChoice == 2) currency = "USD";
            else currency = "EUR";
            // --- КІНЕЦЬ НОВОГО КОДУ ---
            
            string newId = manager.generateAccId();

			string newId = manager.generateAccId();
			if (type == 2) {
                cout << ((lang == AppLanguage::Ukrainian) ? "Кредитний ліміт: " : "Credit limit: ");
                double limit = getValidDouble();
                
                // --- ДОДАЄМО ЗАПИТ ВЛАСНИХ КОШТІВ ---
                cout << ((lang == AppLanguage::Ukrainian) ? "Власні кошти (початковий баланс): " : "Own funds (initial balance): ");
                double initialBalance = getValidDouble();
                
                // Створюємо картку
                auto newCard = make_shared<CreditCard>(newId, sanitize(name), currency, sanitize(currentUser), limit);
                
                // Якщо ти ввела більше нуля, одразу кладемо ці гроші на рахунок
                if (initialBalance > 0) {
                    newCard->deposit(initialBalance);
                }
                
                manager.addAccount(newCard);
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
			showFastAccountList(manager, currentUser, lang);
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
			cout << ((lang == AppLanguage::Ukrainian) ? "Початкова дата (DD.MM.YYYY) (0-вихід): " : "Start date (DD.MM.YYYY) (0-exit): ");
			string startDate = getValidDate(lang);
			if (startDate == "0") continue;
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
			cout << "Оберіть мову / Choose language:\n0-Вихід (Exit)\n1. Українська\n2. English\n> ";
			int newLangChoice;
			while (!(cin >> newLangChoice) || (newLangChoice < 0 || newLangChoice > 2)) {
				cout << "Помилка вводу. 0, 1 або 2: ";
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			if (newLangChoice == 0) continue;
			lang = (newLangChoice == 2) ? AppLanguage::English : AppLanguage::Ukrainian;
			cout << ((lang == AppLanguage::Ukrainian) ? "-> Мову змінено!\n" : "-> Language changed!\n");
			waitUser();
		}
		else if (choice == 8) {
			clearScreen();
			cout << ((lang == AppLanguage::Ukrainian) ? "Початкова дата (DD.MM.YYYY) (0-вихід): " : "Start date (DD.MM.YYYY) (0-exit): ");
			string startDate = getValidDate(lang);
			if (startDate == "0") continue;
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
		else if (choice == 9) {
			clearScreen();
			showFastAccountList(manager, currentUser, lang);
			if (lang == AppLanguage::Ukrainian) {
				cout << "=== ВИДАЛЕННЯ РАХУНКУ ===\n";
				cout << "Введіть ID рахунку для видалення (або '0' для скасування): ";
			}
			else {
				cout << "=== DELETE ACCOUNT ===\n";
				cout << "Enter Account ID to delete (or '0' to cancel): ";
			}

			string accId;
			cin >> accId;

			if (accId != "0") {
				if (manager.deleteAccount(accId, currentUser)) {
					StorageManager::saveToFile(manager, dbFilename);
				}
			}
		}
		else if (choice == 10) {
			clearScreen();
			showFastAccountList(manager, currentUser, lang);
			if (lang == AppLanguage::Ukrainian) {
				cout << "=== РЕДАГУВАННЯ РАХУНКУ ===\n";
				cout << "Введіть ID рахунку для зміни (0-вихід): ";
			}
			else {
				cout << "=== EDIT ACCOUNT ===\n";
				cout << "Enter Account ID to edit (0-exit): ";
			}

			string accId;
			cin >> accId;
			if (accId == "0") continue;
			auto acc = manager.getAccountById(accId);
			if (acc && acc->getOwner() == currentUser) {
				cin.ignore();

				// Запит нової назви
				if (lang == AppLanguage::Ukrainian) cout << "Нова назва рахунку: ";
				else cout << "New account name: ";
				string newName;
				getline(cin, newName);

				double newLimit = -1.0;
				// Якщо це кредитка, запитуємо новий ліміт
				if (acc->getType() == "CREDIT_CARD") {
					if (lang == AppLanguage::Ukrainian) cout << "Новий кредитний ліміт: ";
					else cout << "New credit limit: ";
					newLimit = getValidDouble();
				}

				if (manager.updateAccount(accId, currentUser, newName, newLimit)) {
					StorageManager::saveToFile(manager, dbFilename);
					// Повідомлення про успіх вже є в методі updateAccount (core.cpp)
				}
			}
			else {
				if (lang == AppLanguage::Ukrainian)
					cout << "-> Помилка! Рахунок не знайдено або ви не є власником.\n";
				else
					cout << "-> Error! Account not found or you are not the owner.\n";
			}
			waitUser();
		}
		else if (choice == 11) {
            clearScreen();
            showFastAccountList(manager, currentUser, lang);
            
            cout << ((lang == AppLanguage::Ukrainian) ? "ID рахунку ВІДПРАВНИКА (0 - для відміни): " : "SENDER Account ID (0 to cancel): ");
            string fromId; 
            cin >> fromId;
            if (fromId == "0") continue;
            
            cout << ((lang == AppLanguage::Ukrainian) ? "ID рахунку ОТРИМУВАЧА: " : "RECEIVER Account ID: ");
            string toId; 
            cin >> toId;

            auto accFrom = manager.getAccountById(fromId);
            auto accTo = manager.getAccountById(toId);

            if (!accFrom || !accTo) {
                cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Один або обидва рахунки не знайдено.\n" : "Error! One or both accounts not found.\n");
                waitUser();
                continue;
            }

            if (accFrom->getCurrency() != accTo->getCurrency()) {
                double rateFrom = CurrencyManager::getInstance().getRate(accFrom->getCurrency());
                double rateTo = CurrencyManager::getInstance().getRate(accTo->getCurrency());
                double effectiveRate = rateFrom / rateTo;
                
                cout << "\n[!] " << ((lang == AppLanguage::Ukrainian) ? "Різні валюти. Поточний курс переказу: " : "Different currencies. Current transfer rate: ") 
                     << "1 " << accFrom->getCurrency() << " = " << effectiveRate << " " << accTo->getCurrency() << "\n\n";
            }

            cout << ((lang == AppLanguage::Ukrainian) ? "Сума переказу: " : "Transfer amount: ");
            double amount = getValidDouble();
            
            cout << ((lang == AppLanguage::Ukrainian) ? "Дата (DD.MM.YYYY): " : "Date (DD.MM.YYYY): ");
            string date = getValidDate(lang);

            if (manager.transferFunds(fromId, toId, amount, date, currentUser)) {
                StorageManager::saveToFile(manager, dbFilename);
                cout << ((lang == AppLanguage::Ukrainian) ? "-> Переказ успішно виконано!\n" : "-> Transfer successful!\n");
            } else {
                cout << ((lang == AppLanguage::Ukrainian) ? "-> Помилка! Недостатньо коштів або немає доступу.\n" : "-> Error! Insufficient funds or access denied.\n");
            }
            waitUser();
        }
        else if (choice == 12) {
            clearScreen();
            cout << ((lang == AppLanguage::Ukrainian) ? "--- ВАШ ЗАГАЛЬНИЙ КАПІТАЛ ---\n" : "--- YOUR TOTAL NET WORTH ---\n");
            
            string targetCurrency;
            cout << ((lang == AppLanguage::Ukrainian) ? "В якій валюті порахувати всі ваші гроші? (UAH, USD, EUR): " : "In which currency to calculate all your money? (UAH, USD, EUR): ");
            cin >> targetCurrency;
            
            transform(targetCurrency.begin(), targetCurrency.end(), targetCurrency.begin(), ::toupper);

            double totalAmount = 0.0;
            auto accounts = manager.getAccounts();
            bool hasAccounts = false;
            
            try {
                for (const auto& acc : accounts) {
                    if (acc->hasAccess(currentUser)) {
                        hasAccounts = true;
                        double converted = CurrencyManager::getInstance().convert(acc->getBalance(), acc->getCurrency(), targetCurrency);
                        totalAmount += converted;
                    }
                }
                
                if (!hasAccounts) {
                    cout << ((lang == AppLanguage::Ukrainian) ? "У вас ще немає рахунків.\n" : "You have no accounts yet.\n");
                } else {
                    cout << ((lang == AppLanguage::Ukrainian) ? "\nВаш загальний баланс з усіх рахунків: " : "\nYour overall balance from all accounts: ") 
                         << totalAmount << " " << targetCurrency << "\n";
                }
            } catch (const invalid_argument& e) {
                cout << ((lang == AppLanguage::Ukrainian) ? "Помилка! Валюта не підтримується. Доступні: UAH, USD, EUR.\n" : "Error! Currency not supported. Available: UAH, USD, EUR.\n");
            }
            
            waitUser();
        }
        else if (choice == 13) {
            clearScreen();
            cout << ((lang == AppLanguage::Ukrainian) ? "--- ОНОВЛЕННЯ КУРСУ ВАЛЮТ ---\n" : "--- UPDATE EXCHANGE RATES ---\n");
            
            cout << ((lang == AppLanguage::Ukrainian) ? "Поточні курси (відносно UAH):\n" : "Current rates (relative to UAH):\n");
            cout << "USD: " << CurrencyManager::getInstance().getRate("USD") << "\n";
            cout << "EUR: " << CurrencyManager::getInstance().getRate("EUR") << "\n\n";

            string curr;
            cout << ((lang == AppLanguage::Ukrainian) ? "Яку валюту хочете оновити? (USD, EUR або '0' для відміни): " : "Which currency to update? (USD, EUR or '0' to cancel): ");
            cin >> curr;
            
            if (curr != "0") {
                transform(curr.begin(), curr.end(), curr.begin(), ::toupper);
                
                if (curr == "UAH") {
                    cout << ((lang == AppLanguage::Ukrainian) ? "Курс базової валюти (UAH) змінити не можна.\n" : "Base currency (UAH) rate cannot be changed.\n");
                } else if (curr == "USD" || curr == "EUR") {
                    cout << ((lang == AppLanguage::Ukrainian) ? "Введіть новий курс (скільки гривень за 1 " + curr + "): " : "Enter new rate (how many UAH for 1 " + curr + "): ");
                    double newRate = getValidDouble();
                    
                    CurrencyManager::getInstance().updateRate(curr, newRate);
                    cout << ((lang == AppLanguage::Ukrainian) ? "-> Курс успішно оновлено!\n" : "-> Rate successfully updated!\n");
                } else {
                    cout << ((lang == AppLanguage::Ukrainian) ? "Невідома валюта!\n" : "Unknown currency!\n");
                }
            }
            waitUser();
        }
    }
    return 0;
}
