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
    system("clear");
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

string getValidDate(AppLanguage lang) {
    string input, stdDate;
    while (true) {
        cin >> input;
        if (input == "0")return "0";
        if (parseAndValidateDate(input, stdDate)) return stdDate;
        if (lang == AppLanguage::Ukrainian) cout << "Некоректна дата! Використовуйте DD.MM.YYYY: ";
        else cout << "Invalid date! Use DD.MM.YYYY: ";
    }
}

string sanitize(string s) {
    replace(s.begin(), s.end(), '|', ' ');
    return s;
}
