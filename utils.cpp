#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include "colors.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <regex>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");   // Windows
#else
    system("clear"); // Linux / macOS
#endif
}

void waitUser() {
    cout << "\nНатисніть Enter для продовження / Press Enter to continue...";
    cin.clear();
    if (cin.rdbuf()->in_avail() > 0) {
        cin.ignore(cin.rdbuf()->in_avail());
    }
    cin.get();
}

double getValidDouble() {
    double value;
    while (true) {
        if (cin >> value && value >= 0) {
            return value;
        }
        else {
            cout << "Помилка! Введіть коректне додатне число / Error! Enter a valid positive number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

bool parseAndValidateDate(const std::string& input, std::string& standardizedDate) {
    int d = 0, m = 0, y = 0;
    char sep1, sep2;

    std::stringstream ss(input);
    ss >> d >> sep1 >> m >> sep2 >> y;

    if (ss.fail() || (sep1 != '.' && sep1 != '/' && sep1 != '-') || sep1 != sep2) {
        std::stringstream ssRev(input);
        ssRev >> y >> sep1 >> m >> sep2 >> d;
        if (ssRev.fail() || (sep1 != '.' && sep1 != '/' && sep1 != '-') || sep1 != sep2) {
            return false;
        }
    }

    if (m < 1 || m > 12) return false;

    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
        daysInMonth[2] = 29;
    }
    
    if (d < 1 || d > daysInMonth[m]) return false;

    std::ostringstream oss;
    oss << y << "-" << std::setfill('0') << std::setw(2) << m << "-" << std::setw(2) << d;
    standardizedDate = oss.str();
    
    return true;
}

string getCurrentDate() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return string(buf);
}

string getValidDate(AppLanguage lang) {
    string input, stdDate;
    while (true) {
        getline(cin, input);
        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\r\n"));
        if (!input.empty()) input.erase(input.find_last_not_of(" \t\r\n") + 1);
        if (input.empty()) return getCurrentDate();
        if (input == "0") return "0";
        if (parseAndValidateDate(input, stdDate)) return stdDate;
        if (lang == AppLanguage::Ukrainian) cout << "Некоректна дата! Використовуйте DD.MM.YYYY: ";
        else cout << "Invalid date! Use DD.MM.YYYY: ";
    }
}

string sanitize(string s) {
    replace(s.begin(), s.end(), '|', ' ');
    return s;
}

string progressBar(double current, double target, int width) {
    double ratio = (target > 0) ? min(current / target, 1.0) : 0.0;
    int filled = (int)(ratio * width);
    string bar = "[";
    for (int i = 0; i < width; i++) {
        bar += (i < filled) ? "\u2588" : "\u2591";
    }
    bar += "] " + to_string((int)(ratio * 100)) + "%";
    return bar;
}

void playDrawerAnimation() {
    auto sleepMs = [](int ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    };

    // Frame 1: Nightstand with all drawers closed
    clearScreen();
    cout << YELLOW
         << "\n\n"
         << "              +-----------------------+\n"
         << "              |                       |\n"
         << "              |    " << CYAN << "$  S A V I N G S" << YELLOW << "   |\n"
         << "              +-----------------------+\n"
         << "              |  +-----------------+  |\n"
         << "              |  |       " << BOLD << "(o)" << RESET << YELLOW << "       |  |\n"
         << "              |  +-----------------+  |\n"
         << "              |  +-----------------+  |\n"
         << "              |  |       " << BOLD << "(o)" << RESET << YELLOW << "       |  |\n"
         << "              |  +-----------------+  |\n"
         << "              +-----------------------+\n"
         << "                 ||              ||\n"
         << RESET;
    sleepMs(700);

    // Frame 2: Bottom drawer sliding out
    clearScreen();
    cout << YELLOW
         << "\n\n"
         << "              +-----------------------+\n"
         << "              |                       |\n"
         << "              |    " << CYAN << "$  S A V I N G S" << YELLOW << "   |\n"
         << "              +-----------------------+\n"
         << "              |  +-----------------+  |\n"
         << "              |  |       " << BOLD << "(o)" << RESET << YELLOW << "       |  |\n"
         << "              |  +-----------------+  |\n"
         << "              +-----------------------+\n"
         << "               \\-----------------------\\\n"
         << "               |  " << GREEN << "$" << YELLOW << "       " << BOLD << "(o)" << RESET << YELLOW << "       " << GREEN << "$" << YELLOW << "  |\n"
         << "               +-----------------------+\n"
         << "                 ||              ||\n"
         << RESET;
    sleepMs(700);
}
