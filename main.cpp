#include <iostream>
#define NOMINMAX
#include <windows.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <limits>
#include <vector>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include "types.h"
#include "utils.h"
#include "models.h"
#include "core.h"
#include "menu.h"
#include "colors.h"
using namespace std;


// Helper to format double without trailing zeros
string fmtDouble(double val) {
	ostringstream oss;
	oss << val;
	return oss.str();
}

// Допоміжна функція для швидкого показу рахунків
void showFastAccountList(AccountManager& manager, const string& currentUser, AppLanguage lang) {
	auto accounts = manager.getAccounts();
	cout << "\n==================================================================================\n";
	if (lang == AppLanguage::Ukrainian) cout << "\t\tДоступні рахунки (ID | Назва | Баланс):\n";
	else cout  << "\n\t\tAvailable accounts (ID | Name | Balance):\n";

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
		if (lang == AppLanguage::Ukrainian) cout << "\t\tРахунків не знайдено.\n";
		else cout << "\t\tNo accounts found.\n";
	}
	cout << "\n==================================================================================\n";
}

string toSortable(const string& date) {
	// Dates are already in YYYY-MM-DD format, which sorts lexicographically
	if (date.length() < 10) return "0";
	return date;
}



string getValidEndDate(const string& startDate, AppLanguage lang) {
	string endDate;
	while (true) {
		cout << ((lang == AppLanguage::Ukrainian) ? "📅 Кінцева дата (DD.MM.YYYY, Enter = сьогодні, 0 - відміна): " : "📅 End date (DD.MM.YYYY, Enter = today, 0 - cancel): ");
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

#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif
	if (!setlocale(LC_ALL, "uk_UA.UTF-8")) {
		if (!setlocale(LC_ALL, "en_US.UTF-8")) {
			setlocale(LC_ALL, "");
		}
	}
	setlocale(LC_ALL, ".UTF-8");
	AccountManager manager;
	SavingsManager savingsManager;
	string dbFilename = "finance_data.txt";
	string savingsFilename = "savings_data.txt";

	StorageManager::loadFromFile(manager, dbFilename);
	StorageManager::loadSavingsFromFile(savingsManager, savingsFilename);

	clearScreen();
	cout<<"Оберіть мову / Choose language : \n1.Українська\n2.English\n> ";
	vector<string> langOptions = { "Українська", "English" };
	
	int langIdx = interactiveMenu("\t\t\tОберіть мову / Choose language:", langOptions);
	AppLanguage lang = (langIdx == 1) ? AppLanguage::English : AppLanguage::Ukrainian;

	string currentUser;
	// Clear input buffer after interactive menu
	cin.clear();
#ifdef _WIN32

	cin.clear();
#else
	tcflush(STDIN_FILENO, TCIFLUSH);
#endif
	while (true) {
		clearScreen();
		cout << "==================================================================================\n";
		cout << "\t\t\t" << ((lang == AppLanguage::Ukrainian) ? "Увійдіть у систему" : "Log in") << "\n";
		cout << "==================================================================================\n";
		cout << "\n\t  " << ((lang == AppLanguage::Ukrainian) ? "Введіть ваше ім'я:" : "Enter your name:") << "\n";
		cout << "\n\t> ";
		getline(cin, currentUser);
		// Видалити пробіли з початку та кінця
		currentUser.erase(0, currentUser.find_first_not_of(" \t"));
		currentUser.erase(currentUser.find_last_not_of(" \t") + 1);
		if (!currentUser.empty()) break;
		cout << "\n\t  " << ((lang == AppLanguage::Ukrainian) ? "Помилка: Ім'я не може бути порожнім!" : "Error: Name cannot be empty!") << "\n";
		cout << "\n==================================================================================\n";
		cout << "\t  " << ((lang == AppLanguage::Ukrainian) ? "Натисніть Enter для повтору..." : "Press Enter to retry...");
		cin.get();
	}

	// ==========================================
	// ГОЛОВНИЙ ЦИКЛ ПРОГРАМИ
	// ==========================================
	while (true) {
		string header = (lang == AppLanguage::Ukrainian)
			? "\t\t\tПоточний користувач: [" + currentUser + "]"
			: "\t\t\tCurrent user: [" + currentUser + "]";

		vector<string> menuOptions;
		if (lang == AppLanguage::Ukrainian) {
			menuOptions = {
				"💼 Рахунки (Перегляд, Створення, Редагування...)",
				"💸 Транзакції (Доходи, Витрати, Перекази)",
				"📊 Історія та Звіти (Виписка, ТОП-3 витрат...)",
				"🌍 Капітал та Курси валют",
				"💰 Заощадження (Особисті, Спільні)",
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
				"💰 Savings (Personal, Shared)",
				"⚙️ Settings (Change language / user)",
				"❌ Exit program"
			};
		}
		int selectedIndex = interactiveMenu(header, menuOptions);

		int mainChoice;
		if (selectedIndex == 6) {
			mainChoice = 0;
		}
		else {
			mainChoice = selectedIndex + 1;
		}

		if (mainChoice == 0) {
			StorageManager::saveToFile(manager, dbFilename);
			StorageManager::saveSavingsToFile(savingsManager, savingsFilename);
			playExitAnimation();
			cout << ((lang == AppLanguage::Ukrainian) ? "До побачення!\n" : "Goodbye!\n");
			break; // Вихід з програми
		}

		// ==========================================
		// ПІДМЕНЮ 1: РАХУНКИ
		// ==========================================
		else if (mainChoice == 1) {
			playBriefcaseAnimation();
			while (true) {
				string header = (lang == AppLanguage::Ukrainian)
					? "\t\t 💼 УПРАВЛІННЯ РАХУНКАМИ "
					: "\t\t 💼 ACCOUNT MANAGEMENT ";

				vector<string> options;
				if (lang == AppLanguage::Ukrainian) {
					options = {
						"📋 Переглянути мої рахунки",
						"➕ Створити новий рахунок",
						"✏️ Редагувати рахунок",
						"🗑️ Видалити рахунок",
						"⬅️ Назад до Головного меню"
					};
				}
				else {
					options = {
						"📋 View my accounts",
						"➕ Create new account",
						"✏️ Edit account",
						"🗑️ Delete account",
						"⬅️ Back to Main Menu"
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
					cout << ((lang == AppLanguage::Ukrainian) ? "\t\t\tВАШІ РАХУНКИ \n" : "\t\t\tYOUR ACCOUNTS \n");
					showFastAccountList(manager, currentUser, lang);
					waitUser();
				}
				else if (sub == 2) { // Створення рахунку
					clearScreen();
					int type;
					{
						string typeHeader = (lang == AppLanguage::Ukrainian) ? "\t\tОберіть тип рахунку:" : "\t\tChoose account type:";
						vector<string> typeOptions;
						if (lang == AppLanguage::Ukrainian) {
							typeOptions = { "💵 Готівка", "💳 Кредитка", "👥 Спільний бюджет", "❌ Відміна" };
						}
						else {
							typeOptions = { "💵 Cash", "💳 Credit Card", "👥 Shared Budget", "❌ Cancel" };
						}
						int typeSel = interactiveMenu(typeHeader, typeOptions);
						if (typeSel == 3) continue;
						type = typeSel + 1; // 0->1, 1->2, 2->3
					}

					cin.clear();
#ifdef _WIN32

					cin.clear();
#else
					tcflush(STDIN_FILENO, TCIFLUSH);
#endif
					string name, currency;
					cout << ((lang == AppLanguage::Ukrainian) ? "Назва рахунку: " : "Account name: ");
					getline(cin, name);

					{
						string currHeader = (lang == AppLanguage::Ukrainian) ? "\t\t\tОберіть валюту:" : "\t\t\tChoose currency:";
						vector<string> currOptions = { "UAH", "USD", "EUR" };
						int currSel = interactiveMenu(currHeader, currOptions);
						if (currSel == 0) currency = "UAH";
						else if (currSel == 1) currency = "USD";
						else currency = "EUR";
					}

					string newId = manager.generateAccId();

					cin.clear();
#ifdef _WIN32

					cin.clear();
#else
					tcflush(STDIN_FILENO, TCIFLUSH);
#endif

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
					cout << ((lang == AppLanguage::Ukrainian) ? "\n\t\tРахунок створено! Ваш ID: " : "\n\t\tAccount created! Your ID: ") << newId << "\n";
					waitUser();
				}
				else if (sub == 3) { // Редагування
					clearScreen();
					vector<shared_ptr<Account>> editableAccounts;
					vector<string> editOptions;
					for (const auto& acc : manager.getAccounts()) {
						if (acc->getOwner() == currentUser) {
							editableAccounts.push_back(acc);
							editOptions.push_back("[" + acc->getId() + "] " + acc->getName() + " (" + fmtDouble(acc->getBalance()) + " " + acc->getCurrency() + ")");
						}
					}
					if (editableAccounts.empty()) {
						cout << ((lang == AppLanguage::Ukrainian) ? "\t\tРахунків не знайдено.\n" : "\t\tNo accounts found.\n");
						waitUser(); continue;
					}
					editOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
					string editHeader = (lang == AppLanguage::Ukrainian) ? "\t\tОберіть рахунок для зміни:" : "\t\tChoose account to edit:";
					int editSel = interactiveMenu(editHeader, editOptions);
					if (editSel == (int)editableAccounts.size()) continue;
					string accId = editableAccounts[editSel]->getId();

					auto acc = manager.getAccountById(accId);
					if (acc && acc->getOwner() == currentUser) {
						cin.clear();
#ifdef _WIN32

						cin.clear();
#else
						tcflush(STDIN_FILENO, TCIFLUSH);
#endif
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
					clearScreen();
					vector<shared_ptr<Account>> deletableAccounts;
					vector<string> delOptions;
					for (const auto& acc : manager.getAccounts()) {
						if (acc->getOwner() == currentUser) {
							deletableAccounts.push_back(acc);
							delOptions.push_back("[" + acc->getId() + "] " + acc->getName() + " (" + fmtDouble(acc->getBalance()) + " " + acc->getCurrency() + ")");
						}
					}
					if (deletableAccounts.empty()) {
						cout << ((lang == AppLanguage::Ukrainian) ? "Рахунків не знайдено.\n" : "No accounts found.\n");
						waitUser(); continue;
					}
					delOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
					string delHeader = (lang == AppLanguage::Ukrainian) ? "\t\tОберіть рахунок для видалення:" : "\t\tChoose account to delete:";
					int delSel = interactiveMenu(delHeader, delOptions);
					if (delSel == (int)deletableAccounts.size()) continue;
					string accId = deletableAccounts[delSel]->getId();
					if (manager.deleteAccount(accId, currentUser)) {
						StorageManager::saveToFile(manager, dbFilename);
					}
					waitUser();
				}
			}
		}

		// ==========================================
		// ПІДМЕНЮ 2: ТРАНЗАКЦІЇ
		// ==========================================
		else if (mainChoice == 2) {
			playDollarSnowAnimation();
			while (true) {
				string header = (lang == AppLanguage::Ukrainian)
					? " \t\t💸 ТРАНЗАКЦІЇ "
					: " \t\t💸 TRANSACTIONS ";

				vector<string> options;
				if (lang == AppLanguage::Ukrainian) {
					options = {
						"💰 Поповнити рахунок (Дохід)",
						"💳 Внести витрату",
						"🔄 Переказ між рахунками",
						"⬅️ Назад до Головного меню"
					};
				}
				else {
					options = {
						"💰 Add income",
						"💳 Add expense",
						"🔄 Transfer funds",
						"⬅️ Back to Main Menu"
					};
				}

				int sel = interactiveMenu(header, options);

				int sub;
				if (sel == 3) sub = 0; // Останній пункт (індекс 3) - це "Назад"
				else sub = sel + 1;
				if (sub == 0) break;

				if (sub == 1 || sub == 2) { // Дохід або Витрата
					clearScreen();
					string category, desc;
					shared_ptr<Account> selectedAcc = nullptr;

					// Build interactive account list
					vector<shared_ptr<Account>> accessibleAccounts;
					vector<string> accOptions;
					for (const auto& acc : manager.getAccounts()) {
						if (acc->hasAccess(currentUser)) {
							accessibleAccounts.push_back(acc);
							string label = "[" + acc->getId() + "] " + acc->getName()
								+ " (" + fmtDouble(acc->getBalance()) + " " + acc->getCurrency() + ")";
							if (acc->isShared()) label += (lang == AppLanguage::Ukrainian) ? " (Спільний)" : " (Shared)";
							accOptions.push_back(label);
						}
					}
					if (accessibleAccounts.empty()) {
						cout << ((lang == AppLanguage::Ukrainian) ? "Рахунків не знайдено.\n" : "No accounts found.\n");
						waitUser(); continue;
					}
					accOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
					string accHeader = (lang == AppLanguage::Ukrainian) ? "Оберіть рахунок:" : "Choose account:";
					int accSel = interactiveMenu(accHeader, accOptions);
					if (accSel == (int)accessibleAccounts.size()) continue;
					selectedAcc = accessibleAccounts[accSel];
					string accId = selectedAcc->getId();

					string actingUser = currentUser;
					if (selectedAcc->isShared()) {
						auto sharedAcc = dynamic_pointer_cast<SharedBudget>(selectedAcc);
						auto members = sharedAcc->getMembers();
						string memberHeader = (lang == AppLanguage::Ukrainian) ? "Хто здійснює операцію?" : "Who is making the transaction?";
						int memberSel = interactiveMenu(memberHeader, members);
						actingUser = members[memberSel];
					}

					cin.clear();
#ifdef _WIN32

					cin.clear();
#else
					tcflush(STDIN_FILENO, TCIFLUSH);
#endif
					cout << ((lang == AppLanguage::Ukrainian) ? "Сума: " : "Amount: ");
					double amount = getValidDouble();
					cout << ((lang == AppLanguage::Ukrainian) ? "Категорія: " : "Category: ");
					getline(cin, category);
					cout << ((lang == AppLanguage::Ukrainian) ? "Опис: " : "Description: ");
					getline(cin, desc);
					cout << ((lang == AppLanguage::Ukrainian) ? "📅 Дата (DD.MM.YYYY, Enter = сьогодні): " : "📅 Date (DD.MM.YYYY, Enter = today): ");
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
					clearScreen();
					// Build interactive sender account list
					vector<shared_ptr<Account>> senderAccounts;
					vector<string> senderOptions;
					for (const auto& acc : manager.getAccounts()) {
						if (acc->hasAccess(currentUser)) {
							senderAccounts.push_back(acc);
							senderOptions.push_back("[" + acc->getId() + "] " + acc->getName()
								+ " (" + fmtDouble(acc->getBalance()) + " " + acc->getCurrency() + ")");
						}
					}
					if (senderAccounts.empty()) {
						cout << ((lang == AppLanguage::Ukrainian) ? "Рахунків не знайдено.\n" : "No accounts found.\n");
						waitUser(); continue;
					}
					senderOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
					string senderHeader = (lang == AppLanguage::Ukrainian) ? "\tОберіть рахунок ВІДПРАВНИКА:" : "\tChoose SENDER account:";
					int senderSel = interactiveMenu(senderHeader, senderOptions);
					if (senderSel == (int)senderAccounts.size()) continue;
					string fromId = senderAccounts[senderSel]->getId();

					// Build interactive receiver account list (all accounts)
					vector<shared_ptr<Account>> receiverAccounts;
					vector<string> receiverOptions;
					for (const auto& acc : manager.getAccounts()) {
						if (acc->getId() != fromId) {
							receiverAccounts.push_back(acc);
							receiverOptions.push_back("[" + acc->getId() + "] " + acc->getName()
								+ " (" + fmtDouble(acc->getBalance()) + " " + acc->getCurrency() + ")");
						}
					}
					if (receiverAccounts.empty()) {
						cout << ((lang == AppLanguage::Ukrainian) ? "Немає інших рахунків.\n" : "No other accounts.\n");
						waitUser(); continue;
					}
					receiverOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
					string receiverHeader = (lang == AppLanguage::Ukrainian) ? "Оберіть рахунок ОТРИМУВАЧА:" : "Choose RECEIVER account:";
					int receiverSel = interactiveMenu(receiverHeader, receiverOptions);
					if (receiverSel == (int)receiverAccounts.size()) continue;
					string toId = receiverAccounts[receiverSel]->getId();

					auto accFrom = manager.getAccountById(fromId);
					auto accTo = manager.getAccountById(toId);

					if (accFrom->getCurrency() != accTo->getCurrency()) {
						double rateFrom = CurrencyManager::getInstance().getRate(accFrom->getCurrency());
						double rateTo = CurrencyManager::getInstance().getRate(accTo->getCurrency());
						cout << "\n[!] " << ((lang == AppLanguage::Ukrainian) ? "Курс переказу: " : "Transfer rate: ")
							<< "1 " << accFrom->getCurrency() << " = " << (rateFrom / rateTo) << " " << accTo->getCurrency() << "\n\n";
					}

					cin.clear();
#ifdef _WIN32

					cin.clear();
#else
					tcflush(STDIN_FILENO, TCIFLUSH);
#endif
					cout << ((lang == AppLanguage::Ukrainian) ? "Сума переказу: " : "Transfer amount: ");
					double amount = getValidDouble();
					cout << ((lang == AppLanguage::Ukrainian) ? "📅 Дата (DD.MM.YYYY, Enter = сьогодні): " : "📅 Date (DD.MM.YYYY, Enter = today): ");
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
			playChartAnimation();
			while (true) {
				string header = (lang == AppLanguage::Ukrainian) ? " \t\t📊 ІСТОРІЯ ТА ЗВІТИ" : "\t\t📊 HISTORY & REPORTS";
				vector<string> options;

				if (lang == AppLanguage::Ukrainian) {
					options = {
						"📜 Виписка по рахунку (Історія транзакцій)",
						"🏆 Звіт: ТОП-3 моїх витрат",
						"👥 Звіт: Витрати по користувачах",
						"📈 Загальний фінансовий звіт",
						"📊 Звіт: ТОП-3 категорій витрат",
						"⬅️ Назад до Головного меню"
					};
				}
				else {
					options = {
						"📜 Account Statement",
						"🏆 Report: TOP-3 expenses",
						"👥 Report: Expenses by user",
						"📈 General Financial Report",
						"📊 Report: TOP-3 expense categories",
						"⬅️ Back to Main Menu"
					};
				}

				int sel = interactiveMenu(header, options);
				int sub = (sel == 5) ? 0 : sel + 1;
				if (sub == 0) break;

				if (sub == 1) {
					clearScreen();
					vector<shared_ptr<Account>> histAccounts;
					vector<string> histOptions;
					for (const auto& acc : manager.getAccounts()) {
						if (acc->hasAccess(currentUser)) {
							histAccounts.push_back(acc);
							string label = "[" + acc->getId() + "] " + acc->getName()
								+ " (" + fmtDouble(acc->getBalance()) + " " + acc->getCurrency() + ")";
							if (acc->isShared()) label += (lang == AppLanguage::Ukrainian) ? " (Спільний)" : " (Shared)";
							histOptions.push_back(label);
						}
					}
					if (histAccounts.empty()) {
						cout << ((lang == AppLanguage::Ukrainian) ? "Рахунків не знайдено.\n" : "No accounts found.\n");
						waitUser(); continue;
					}
					histOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
					string histHeader = (lang == AppLanguage::Ukrainian) ? "Оберіть рахунок:" : "Choose account:";
					int histSel = interactiveMenu(histHeader, histOptions);
					if (histSel == (int)histAccounts.size()) continue;
					string accId = histAccounts[histSel]->getId();
					{
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
					cout << ((lang == AppLanguage::Ukrainian) ? "📅 Початкова дата (DD.MM.YYYY, Enter = сьогодні, 0 - відміна): " : "📅 Start date (DD.MM.YYYY, Enter = today, 0 - cancel): ");
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
					cout << ((lang == AppLanguage::Ukrainian) ? "📅 Початкова дата (DD.MM.YYYY, Enter = сьогодні, 0 - відміна): " : "📅 Start date (DD.MM.YYYY, Enter = today, 0 - cancel): ");
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

					cout << ((lang == AppLanguage::Ukrainian) ? "📅 Початкова дата (DD.MM.YYYY, Enter = сьогодні, 0 - відміна): " : "📅 Start date (DD.MM.YYYY, Enter = today, 0 - cancel): ");
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
			playCurrencyAnimation();
			while (true) {
				string header = (lang == AppLanguage::Ukrainian) ? "\t\t🌍 КАПІТАЛ ТА ВАЛЮТИ " : "\t\t🌍 TOTAL NET WORTH & CURRENCIES ";
				vector<string> options;

				if (lang == AppLanguage::Ukrainian) {
					options = { "💎 Розрахувати мій Загальний капітал", "💱 Оновити курси валют", "⬅️ Назад" };
				}
				else {
					options = { "💎 Calculate Total Net Worth", "💱 Update exchange rates", "⬅️ Back" };
				}

				int sel = interactiveMenu(header, options);
				int sub = (sel == 2) ? 0 : sel + 1;
				if (sub == 0) break;

				if (sub == 1) { // Капітал
					clearScreen();
					{
						string currHeader = (lang == AppLanguage::Ukrainian) ? "Оберіть валюту:" : "Choose currency:";
						vector<string> currOptions = { "UAH", "USD", "EUR", ((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel") };
						int currSel = interactiveMenu(currHeader, currOptions);
						if (currSel == 3) continue;
						std::string currencyStr;
						if (currSel == 0) currencyStr = "UAH";
						else if (currSel == 1) currencyStr = "USD";
						else currencyStr = "EUR";
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
					}
					waitUser();
				}
				else if (sub == 2) { // Оновити курси
					clearScreen();
					cout << ((lang == AppLanguage::Ukrainian) ? "Поточні курси (до UAH):\n" : "Current rates (to UAH):\n");
					cout << "USD: " << CurrencyManager::getInstance().getRate("USD") << "\n";
					cout << "EUR: " << CurrencyManager::getInstance().getRate("EUR") << "\n\n";

					{
						string rateHeader = (lang == AppLanguage::Ukrainian) ? "Яку валюту оновити?" : "Currency to update?";
						vector<string> rateOptions = { "USD", "EUR", ((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel") };
						int rateSel = interactiveMenu(rateHeader, rateOptions);
						if (rateSel == 2) continue;
						string curr = (rateSel == 0) ? "USD" : "EUR";
						cin.clear();
#ifdef _WIN32

						cin.clear();
#else
						tcflush(STDIN_FILENO, TCIFLUSH);
#endif
						cout << ((lang == AppLanguage::Ukrainian) ? "Новий курс: " : "New rate: ");
						double newRate = getValidDouble();
						CurrencyManager::getInstance().updateRate(curr, newRate);
						cout << "-> OK!\n";
					}
					waitUser();
				}
			}
		}

		// ==========================================
		// ПІДМЕНЮ 5: ЗАОЩАДЖЕННЯ
		// ==========================================
		else if (mainChoice == 5) {
			playDrawerAnimation();
			while (true) {
				string header = (lang == AppLanguage::Ukrainian)
					? "\t\t💰 ЗАОЩАДЖЕННЯ "
					: "\t\t💰 SAVINGS ";

				vector<string> options;
				if (lang == AppLanguage::Ukrainian) {
					options = {
						"🏦 Особисті заощадження",
						"🤝 Спільні заощадження",
						"⬅️ Назад до Головного меню"
					};
				}
				else {
					options = {
						"🏦 Personal Savings",
						"🤝 Shared Savings",
						"⬅️ Back to Main Menu"
					};
				}

				int sel = interactiveMenu(header, options);
				if (sel == 2) break;

				bool isShared = (sel == 1);

				while (true) {
					string subHeader = (lang == AppLanguage::Ukrainian)
						? (isShared ? "\t\t🤝 СПІЛЬНІ ЗАОЩАДЖЕННЯ " : "\t\t🏦 ОСОБИСТІ ЗАОЩАДЖЕННЯ ")
						: (isShared ? "\t\t🤝 SHARED SAVINGS " : "\t\t🏦 PERSONAL SAVINGS ");	

					vector<string> subOptions;
					if (lang == AppLanguage::Ukrainian) {
						subOptions = {
							"📋 Переглянути цілі",
							"➕ Створити нову ціль",
							"💵 Поповнити заощадження",
							"💸 Зняти кошти",
							"✏️ Редагувати ціль"
						};
						if (isShared) subOptions.push_back("👥 Керувати учасниками");
						subOptions.push_back("🗑️ Видалити ціль");
						subOptions.push_back("⬅️ Назад");
					}
					else {
						subOptions = {
							"📋 View goals",
							"➕ Create new goal",
							"💵 Add funds",
							"💸 Withdraw funds",
							"✏️ Edit goal"
						};
						if (isShared) subOptions.push_back("👥 Manage members");
						subOptions.push_back("🗑️ Delete goal");
						subOptions.push_back("⬅️ Back");
					}

					int subSel = interactiveMenu(subHeader, subOptions);
					int backIdx = (int)subOptions.size() - 1;
					int deleteIdx = backIdx - 1;
					int membersIdx = isShared ? (deleteIdx - 1) : -1;

					if (subSel == backIdx) break;

					// === VIEW ===
					if (subSel == 0) {
						clearScreen();
						cout << ((lang == AppLanguage::Ukrainian)
							? (isShared ? "\t\t🤝 СПІЛЬНІ ЦІЛІ\n" : "\t\t🏦 ОСОБИСТІ ЦІЛІ\n")
							: (isShared ? "\t\t🤝 SHARED GOALS\n" : "\t\t🏦 PERSONAL GOALS\n"));
						cout << "\n==================================================================================\n";
						bool found = false;
						for (auto& goal : savingsManager.getGoals()) {
							if (goal.isShared() != isShared) continue;
							if (!goal.hasAccess(currentUser)) continue;
							found = true;
							cout << " [" << goal.getId() << "] " << goal.getName()
								<< " | " << goal.getCurrentAmount() << "/" << goal.getTargetAmount()
								<< " " << goal.getCurrency();
							if (isShared) {
								auto members = goal.getMembers();
								cout << " | 👥 ";
								for (size_t i = 0; i < members.size(); ++i) {
									cout << members[i];
									if (i < members.size() - 1) cout << ", ";
								}
							}
							cout << "\n   " << progressBar(goal.getCurrentAmount(), goal.getTargetAmount());
							if (!goal.getDeadline().empty() && goal.getDeadline() != "0") {
								cout << " | 📅 " << goal.getDeadline();
							}
							cout << "\n";
						}
						if (!found) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Цілей не знайдено.\n" : "No goals found.\n");
						}
						cout << "\n==================================================================================\n";
						waitUser();
					}

					// === CREATE ===
					else if (subSel == 1) {
						clearScreen();
						string name, currency, deadline;
						cin.clear();
#ifdef _WIN32

						cin.clear();
#else
						tcflush(STDIN_FILENO, TCIFLUSH);
#endif
						cout << ((lang == AppLanguage::Ukrainian) ? "Назва цілі: " : "Goal name: ");
						getline(cin, name);
						if (name.empty()) { cout << ((lang == AppLanguage::Ukrainian) ? "Помилка!\n" : "Error!\n"); waitUser(); continue; }

						cout << ((lang == AppLanguage::Ukrainian) ? "Цільова сума: " : "Target amount: ");
						double target = getValidDouble();

						{
							string currHeader = (lang == AppLanguage::Ukrainian) ? "Оберіть валюту:" : "Choose currency:";
							vector<string> currOptions = { "UAH", "USD", "EUR" };
							int currSel = interactiveMenu(currHeader, currOptions);
							if (currSel == 0) currency = "UAH";
							else if (currSel == 1) currency = "USD";
							else currency = "EUR";
						}

						cin.clear();
#ifdef _WIN32

						cin.clear();
#else
						tcflush(STDIN_FILENO, TCIFLUSH);
#endif
						cout << ((lang == AppLanguage::Ukrainian) ? "📅 Дедлайн (DD.MM.YYYY, Enter = сьогодні, 0 - без дедлайну): " : "📅 Deadline (DD.MM.YYYY, Enter = today, 0 - no deadline): ");
						deadline = getValidDate(lang);

						string newId = savingsManager.generateGoalId();
						SavingsGoal newGoal(newId, sanitize(name), target, 0.0, currency, currentUser, deadline, isShared);

						if (isShared) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Введіть імена учасників через пробіл: " : "Enter member names separated by space: ");
							string usersLine; getline(cin, usersLine);
							stringstream ss(usersLine); string u; vector<string> members;
							while (ss >> u) members.push_back(sanitize(u));
							if (find(members.begin(), members.end(), currentUser) == members.end())
								members.push_back(sanitize(currentUser));
							newGoal.setMembers(members);
						}

						savingsManager.addGoal(newGoal);
						StorageManager::saveSavingsToFile(savingsManager, savingsFilename);
						cout << ((lang == AppLanguage::Ukrainian) ? "✅ Ціль створено! ID: " : "✅ Goal created! ID: ") << newId << "\n";
						waitUser();
					}

					// === ADD FUNDS ===
					else if (subSel == 2) {
						clearScreen();
						vector<SavingsGoal*> fundGoals;
						vector<string> fundOptions;
						for (auto& goal : savingsManager.getGoals()) {
							if (goal.isShared() != isShared || !goal.hasAccess(currentUser)) continue;
							fundGoals.push_back(&goal);
							fundOptions.push_back("[" + goal.getId() + "] " + goal.getName()
								+ " (" + fmtDouble(goal.getCurrentAmount()) + "/" + fmtDouble(goal.getTargetAmount()) + " " + goal.getCurrency() + ")");
						}
						if (fundGoals.empty()) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Цілей не знайдено.\n" : "No goals found.\n");
							waitUser(); continue;
						}
						fundOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
						string fundHeader = (lang == AppLanguage::Ukrainian) ? "\t\t💵 ПОПОВНЕННЯ" : "\t\t💵 ADD FUNDS";
						int fundSel = interactiveMenu(fundHeader, fundOptions);
						if (fundSel == (int)fundGoals.size()) continue;

						auto* goal = fundGoals[fundSel];
						cin.clear();
#ifdef _WIN32

						cin.clear();
#else
						tcflush(STDIN_FILENO, TCIFLUSH);
#endif
						cout << ((lang == AppLanguage::Ukrainian) ? "Сума поповнення: " : "Amount to add: ");
						double amount = getValidDouble();
						goal->addFunds(amount);
						StorageManager::saveSavingsToFile(savingsManager, savingsFilename);
						cout << ((lang == AppLanguage::Ukrainian) ? "✅ Поповнено! " : "✅ Added! ")
							<< progressBar(goal->getCurrentAmount(), goal->getTargetAmount()) << "\n";
						waitUser();
					}

					// === WITHDRAW ===
					else if (subSel == 3) {
						clearScreen();
						vector<SavingsGoal*> wdGoals;
						vector<string> wdOptions;
						for (auto& goal : savingsManager.getGoals()) {
							if (goal.isShared() != isShared || !goal.hasAccess(currentUser)) continue;
							wdGoals.push_back(&goal);
							wdOptions.push_back("[" + goal.getId() + "] " + goal.getName()
								+ " (" + fmtDouble(goal.getCurrentAmount()) + " " + goal.getCurrency() + ")");
						}
						if (wdGoals.empty()) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Цілей не знайдено.\n" : "No goals found.\n");
							waitUser(); continue;
						}
						wdOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
						string wdHeader = (lang == AppLanguage::Ukrainian) ? "\t\t💸 ЗНЯТТЯ КОШТІВ" : "\t\t💸 WITHDRAW";
						int wdSel = interactiveMenu(wdHeader, wdOptions);
						if (wdSel == (int)wdGoals.size()) continue;

						auto* goal = wdGoals[wdSel];
						cin.clear();
#ifdef _WIN32

						cin.clear();
#else
						tcflush(STDIN_FILENO, TCIFLUSH);
#endif
						cout << ((lang == AppLanguage::Ukrainian) ? "Сума зняття (макс " : "Withdraw amount (max ")
							<< goal->getCurrentAmount() << "): ";
						double amount = getValidDouble();
						if (goal->withdrawFunds(amount)) {
							StorageManager::saveSavingsToFile(savingsManager, savingsFilename);
							cout << ((lang == AppLanguage::Ukrainian) ? "✅ Знято!\n" : "✅ Withdrawn!\n");
						}
						else {
							cout << ((lang == AppLanguage::Ukrainian) ? "❌ Недостатньо коштів!\n" : "❌ Insufficient funds!\n");
						}
						waitUser();
					}

					// === EDIT ===
					else if (subSel == 4) {
						clearScreen();
						vector<SavingsGoal*> editGoals;
						vector<string> editGoalOptions;
						for (auto& goal : savingsManager.getGoals()) {
							if (goal.isShared() != isShared || !goal.hasAccess(currentUser)) continue;
							editGoals.push_back(&goal);
							editGoalOptions.push_back("[" + goal.getId() + "] " + goal.getName()
								+ " | 🎯 " + fmtDouble(goal.getTargetAmount()) + " " + goal.getCurrency()
								+ " | 📅 " + goal.getDeadline());
						}
						if (editGoals.empty()) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Цілей не знайдено.\n" : "No goals found.\n");
							waitUser(); continue;
						}
						editGoalOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
						string editGoalHeader = (lang == AppLanguage::Ukrainian) ? "\t\t✏️ РЕДАГУВАННЯ" : "\t\t✏️ EDIT GOAL";
						int editGoalSel = interactiveMenu(editGoalHeader, editGoalOptions);
						if (editGoalSel == (int)editGoals.size()) continue;

						auto* goal = editGoals[editGoalSel];
						if (goal->getOwner() != currentUser) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Помилка доступу!\n" : "Access denied!\n");
							waitUser(); continue;
						}
						cin.clear();
#ifdef _WIN32

						cin.clear();
#else
						tcflush(STDIN_FILENO, TCIFLUSH);
#endif
						cout << ((lang == AppLanguage::Ukrainian) ? "Нова назва (Enter - без змін): " : "New name (Enter - no change): ");
						string newName; getline(cin, newName);
						if (!newName.empty()) goal->setName(sanitize(newName));

						cout << ((lang == AppLanguage::Ukrainian) ? "Нова цільова сума (0 - без змін): " : "New target amount (0 - no change): ");
						double newTarget = getValidDouble();
						if (newTarget > 0) goal->setTargetAmount(newTarget);

						cout << ((lang == AppLanguage::Ukrainian) ? "📅 Новий дедлайн (DD.MM.YYYY, Enter = сьогодні, 0 - без змін): " : "📅 New deadline (DD.MM.YYYY, Enter = today, 0 - no change): ");
						string newDeadline = getValidDate(lang);
						if (newDeadline != "0") goal->setDeadline(newDeadline);

						StorageManager::saveSavingsToFile(savingsManager, savingsFilename);
						cout << ((lang == AppLanguage::Ukrainian) ? "✅ Ціль оновлено!\n" : "✅ Goal updated!\n");
						waitUser();
					}

					// === MANAGE MEMBERS (shared only) ===
					else if (isShared && subSel == membersIdx) {
						clearScreen();
						vector<SavingsGoal*> memberGoals;
						vector<string> memberGoalOptions;
						for (auto& goal : savingsManager.getGoals()) {
							if (!goal.isShared() || !goal.hasAccess(currentUser)) continue;
							memberGoals.push_back(&goal);
							string label = "[" + goal.getId() + "] " + goal.getName() + " | 👥 ";
							auto members = goal.getMembers();
							for (size_t i = 0; i < members.size(); ++i) {
								label += members[i];
								if (i < members.size() - 1) label += ", ";
							}
							memberGoalOptions.push_back(label);
						}
						if (memberGoals.empty()) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Цілей не знайдено.\n" : "No goals found.\n");
							waitUser(); continue;
						}
						memberGoalOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
						string mgHeader = (lang == AppLanguage::Ukrainian) ? "\t\t👥 КЕРУВАННЯ УЧАСНИКАМИ" : "\t\t👥 MANAGE MEMBERS";
						int mgSel = interactiveMenu(mgHeader, memberGoalOptions);
						if (mgSel == (int)memberGoals.size()) continue;

						auto* goal = memberGoals[mgSel];
						if (goal->getOwner() != currentUser) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Помилка доступу! Тільки власник може керувати.\n" : "Access denied! Only owner can manage.\n");
							waitUser(); continue;
						}

						{
							string actionHeader = (lang == AppLanguage::Ukrainian) ? "Оберіть дію:" : "Choose action:";
							vector<string> actionOptions;
							if (lang == AppLanguage::Ukrainian) {
								actionOptions = { "Додати учасника", "Видалити учасника", "Відміна" };
							}
							else {
								actionOptions = { "Add member", "Remove member", "Cancel" };
							}
							int actionSel = interactiveMenu(actionHeader, actionOptions);
							if (actionSel == 2) { StorageManager::saveSavingsToFile(savingsManager, savingsFilename); continue; }

							if (actionSel == 0) {
								cin.clear();
#ifdef _WIN32

								cin.clear();
#else
								tcflush(STDIN_FILENO, TCIFLUSH);
#endif
								cout << ((lang == AppLanguage::Ukrainian) ? "Ім'я нового учасника: " : "New member name: ");
								string name; getline(cin, name);
								goal->addMember(sanitize(name));
								cout << ((lang == AppLanguage::Ukrainian) ? "✅ Учасника додано!\n" : "✅ Member added!\n");
							}
							else {
								auto members = goal->getMembers();
								// Filter out owner from removable members
								vector<string> removableMembers;
								for (const auto& m : members) {
									if (m != goal->getOwner()) removableMembers.push_back(m);
								}
								if (removableMembers.empty()) {
									cout << ((lang == AppLanguage::Ukrainian) ? "❌ Немає учасників для видалення!\n" : "❌ No members to remove!\n");
								}
								else {
									removableMembers.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
									string rmHeader = (lang == AppLanguage::Ukrainian) ? "Оберіть учасника для видалення:" : "Choose member to remove:";
									int rmSel = interactiveMenu(rmHeader, removableMembers);
									if (rmSel < (int)removableMembers.size() - 1) {
										goal->removeMember(removableMembers[rmSel]);
										cout << ((lang == AppLanguage::Ukrainian) ? "✅ Учасника видалено!\n" : "✅ Member removed!\n");
									}
								}
							}
						}
						StorageManager::saveSavingsToFile(savingsManager, savingsFilename);
						waitUser();
					}

					// === DELETE ===
					else if (subSel == deleteIdx) {
						clearScreen();
						vector<string> delGoalIds;
						vector<string> delGoalOptions;
						for (auto& goal : savingsManager.getGoals()) {
							if (goal.isShared() != isShared || goal.getOwner() != currentUser) continue;
							delGoalIds.push_back(goal.getId());
							delGoalOptions.push_back("[" + goal.getId() + "] " + goal.getName()
								+ " (" + fmtDouble(goal.getCurrentAmount()) + "/" + fmtDouble(goal.getTargetAmount()) + " " + goal.getCurrency() + ")");
						}
						if (delGoalIds.empty()) {
							cout << ((lang == AppLanguage::Ukrainian) ? "Цілей не знайдено.\n" : "No goals found.\n");
							waitUser(); continue;
						}
						delGoalOptions.push_back((lang == AppLanguage::Ukrainian) ? "Відміна" : "Cancel");
						string delGoalHeader = (lang == AppLanguage::Ukrainian) ? "\t\t🗑️ ВИДАЛЕННЯ" : "\t\t🗑️ DELETE GOAL";
						int delGoalSel = interactiveMenu(delGoalHeader, delGoalOptions);
						if (delGoalSel == (int)delGoalIds.size()) continue;

						string gId = delGoalIds[delGoalSel];
						if (savingsManager.deleteGoal(gId, currentUser)) {
							StorageManager::saveSavingsToFile(savingsManager, savingsFilename);
							cout << ((lang == AppLanguage::Ukrainian) ? "✅ Ціль видалено!\n" : "✅ Goal deleted!\n");
						}
						else {
							cout << ((lang == AppLanguage::Ukrainian) ? "❌ Помилка! Ціль не знайдено або доступ заборонено.\n" : "❌ Error! Goal not found or access denied.\n");
						}
						waitUser();
					}
				}
			}
		}

		// ==========================================
		// ПІДМЕНЮ 6: НАЛАШТУВАННЯ
		// ==========================================
		else if (mainChoice == 6) {
			playSettingsAnimation();
			while (true) {
				string header = (lang == AppLanguage::Ukrainian) ? "\t\t⚙️ НАЛАШТУВАННЯ" : "\t\t⚙️ SETTINGS";
				vector<string> options;

				if (lang == AppLanguage::Ukrainian) {
					options = { "👤 Змінити користувача", "🌐 Змінити мову інтерфейсу", "⬅️ Назад" };
				}
				else {
					options = { "👤 Change user", "🌐 Change interface language", "⬅️ Back" };
				}

				int sel = interactiveMenu(header, options);
				int sub = (sel == 2) ? 0 : sel + 1;

				if (sub == 0) break;

				if (sub == 1) { // Змінити користувача
					clearScreen();
					cout << ((lang == AppLanguage::Ukrainian) ? "Введіть ім'я (0 - вихід):\n> " : "Enter name (0 - exit):\n> ");
					string newUser;
					getline(cin, newUser);
					newUser.erase(0, newUser.find_first_not_of(" \t"));
					newUser.erase(newUser.find_last_not_of(" \t") + 1);
					if (newUser == "0" || newUser.empty()) continue;
					currentUser = sanitize(newUser);
					cout << "-> OK: " << currentUser << "!\n";
					waitUser(); break; // Виходимо в головне меню після зміни
				}
				else if (sub == 2) { // Змінити мову
					string langHeader = (lang == AppLanguage::Ukrainian)
						? "\tОБЕРІТЬ МОВУ"
						: "\tCHOOSE LANGUAGE";

					vector<string> langOptions;
					if (lang == AppLanguage::Ukrainian) {
						langOptions = { "1. 🇺🇦 Українська", "2. 🇬🇧 English", "❌ Відміна" };
					}
					else {
						langOptions = { "1. 🇺🇦 Ukrainian", "2. 🇬🇧 English", "❌ Cancel" };
					}

					// Викликаємо наше інтерактивне меню
					int langIdx = interactiveMenu(langHeader, langOptions);

					// Обробка вибору:
					// langIdx 0 -> Українська
					// langIdx 1 -> English
					// langIdx 2 -> Відміна (0)

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
						// Користувач вибрав "Відміна", просто повертаємось назад
						continue;
					}
				}
			}
		}
	}
	return 0;
}
