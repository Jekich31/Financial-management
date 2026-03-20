#include "utils.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <regex>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

void clearScreen() {
    system("cls"); // Для Windows
}

void waitUser() {
    cout << "\nНатисніть Enter для продовження / Press Enter to continue...";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

bool parseAndValidateDate(const string& input, string& standardizedDate) {
    smatch match;
    regex formatDayFirst("^(\\d{1,2})[./-](\\d{1,2})[./-](\\d{4})$");
    regex formatYearFirst("^(\\d{4})[./-](\\d{1,2})[./-](\\d{1,2})$");

    int d = 0, m = 0, y = 0;
    if (regex_match(input, match, formatDayFirst)) {
        d = stoi(match[1]); m = stoi(match[2]); y = stoi(match[3]);
    }
    else if (regex_match(input, match, formatYearFirst)) {
        y = stoi(match[1]); m = stoi(match[2]); d = stoi(match[3]);
    }
    else {
        return false;
    }

    if (m < 1 || m > 12) return false;

    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) daysInMonth[2] = 29;
    if (d < 1 || d > daysInMonth[m]) return false;

    ostringstream oss;
    oss << y << "-" << setfill('0') << setw(2) << m << "-" << setw(2) << d;
    standardizedDate = oss.str();
    return true;
}

string getValidDate(AppLanguage lang) {
    string input, stdDate;
    while (true) {
        cin >> input;
        if (parseAndValidateDate(input, stdDate)) return stdDate;
        if (lang == AppLanguage::Ukrainian) cout << "Некоректна дата! Використовуйте DD.MM.YYYY: ";
        else cout << "Invalid date! Use DD.MM.YYYY: ";
    }
}

string sanitize(string s) {
    replace(s.begin(), s.end(), '|', ' ');
    return s;
}
