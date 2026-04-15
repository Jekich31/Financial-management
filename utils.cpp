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

void playBriefcaseAnimation() {
    auto sleepMs = [](int ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    };

    // Frame 1: Triangle upright, left position
    clearScreen();
    cout << "\n\n\n"
         << "           \xF0\x9F\x92\xB3\n"
         << "\n"
         << "\n"
         << "        \xF0\x9F\x92\xB0      \xF0\x9F\x92\xB5\n"
         << "\n\n" << RESET;
    sleepMs(400);

    // Frame 2: Tipping right, balanced on bottom vertex
    clearScreen();
    cout << "\n\n\n"
         << "            \xF0\x9F\x92\xB0   \xF0\x9F\x92\xB3\n"
         << "\n"
         << "              \xF0\x9F\x92\xB5\n"
         << "\n\n\n" << RESET;
    sleepMs(300);

    // Frame 3: Triangle upright, center position
    clearScreen();
    cout << "\n\n\n"
         << "                \xF0\x9F\x92\xB0\n"
         << "\n"
         << "\n"
         << "             \xF0\x9F\x92\xB5      \xF0\x9F\x92\xB3\n"
         << "\n\n" << RESET;
    sleepMs(400);

    // Frame 4: Tipping right again
    clearScreen();
    cout << "\n\n\n"
         << "                 \xF0\x9F\x92\xB5   \xF0\x9F\x92\xB0\n"
         << "\n"
         << "                   \xF0\x9F\x92\xB3\n"
         << "\n\n\n" << RESET;
    sleepMs(300);

    // Frame 5: Triangle upright, right position + title
    clearScreen();
    cout << "\n\n\n"
         << "                     \xF0\x9F\x92\xB5\n"
         << "\n"
         << "\n"
         << "                  \xF0\x9F\x92\xB3      \xF0\x9F\x92\xB0\n"
         << "\n"
         << "              " << CYAN << BOLD << "A  C  C  O  U  N  T  S" << RESET << "\n"
         << RESET;
    sleepMs(500);
}

void playChartAnimation() {
    auto sleepMs = [](int ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    };

    // Frame 1: empty axes
    clearScreen();
    cout << "\n\n"
         << "               |\n"
         << "               |\n"
         << "               |\n"
         << "               |\n"
         << "               |\n"
         << "               |\n"
         << "               |______________________________\n"
         << "\n\n" << RESET;
    sleepMs(400);

    // Frame 2: small bars appear
    clearScreen();
    cout << "\n\n"
         << "               |\n"
         << "               |\n"
         << "               |\n"
         << "               |\n"
         << "               |         " << CYAN << "####" << RESET << "\n"
         << "               |  " << GREEN << "####" << RESET << "  " << CYAN << "####" << RESET << "        " << YELLOW << "####" << RESET << "\n"
         << "               |__" << GREEN << "####" << RESET << "__" << CYAN << "####" << RESET << "________" << YELLOW << "####" << RESET << "__\n"
         << "\n\n" << RESET;
    sleepMs(350);

    // Frame 3: bars grow
    clearScreen();
    cout << "\n\n"
         << "               |\n"
         << "               |                " << GREEN << "####" << RESET << "\n"
         << "               |         " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "\n"
         << "               |  " << GREEN << "####" << RESET << "  " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "  " << YELLOW << "####" << RESET << "\n"
         << "               |  " << GREEN << "####" << RESET << "  " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "  " << YELLOW << "####" << RESET << "\n"
         << "               |  " << GREEN << "####" << RESET << "  " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "  " << YELLOW << "####" << RESET << "  " << CYAN << "####" << RESET << "\n"
         << "               |__" << GREEN << "####" << RESET << "__" << CYAN << "####" << RESET << "__" << GREEN << "####" << RESET << "__" << YELLOW << "####" << RESET << "__" << CYAN << "####" << RESET << "__\n"
         << "\n\n" << RESET;
    sleepMs(350);

    // Frame 4: full chart + title
    clearScreen();
    cout << "\n\n"
         << "               |               " << GREEN << "####" << RESET << "\n"
         << "               |         " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "\n"
         << "               |         " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "         " << CYAN << "####" << RESET << "\n"
         << "               |  " << GREEN << "####" << RESET << "  " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "  " << YELLOW << "####" << RESET << "  " << CYAN << "####" << RESET << "\n"
         << "               |  " << GREEN << "####" << RESET << "  " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "  " << YELLOW << "####" << RESET << "  " << CYAN << "####" << RESET << "\n"
         << "               |  " << GREEN << "####" << RESET << "  " << CYAN << "####" << RESET << "  " << GREEN << "####" << RESET << "  " << YELLOW << "####" << RESET << "  " << CYAN << "####" << RESET << "\n"
         << "               |__" << GREEN << "####" << RESET << "__" << CYAN << "####" << RESET << "__" << GREEN << "####" << RESET << "__" << YELLOW << "####" << RESET << "__" << CYAN << "####" << RESET << "__\n"
         << "\n"
         << "                    " << CYAN << BOLD << "R  E  P  O  R  T  S" << RESET << "\n"
         << RESET;
    sleepMs(500);
}

void playDollarSnowAnimation() {
    auto sleepMs = [](int ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    };

    const int W = 55;
    const int H = 13;
    const int TITLE_ROW = 6;
    const string title = "$ T R A N S A C T I O N S $";
    const int tStart = (W - (int)title.size()) / 2;
    const int tEnd = tStart + (int)title.size();

    int startPos[][2] = {
        {0, 3},  {1, 19}, {2, 38}, {0, 50},
        {4, 10}, {3, 29}, {5, 46},
        {7, 5},  {8, 24}, {7, 48},
        {10,12}, {9, 35}, {11, 7},
        {12,42}, {1, 52}
    };
    const int N = 15;

    for (int f = 0; f < 4; f++) {
        clearScreen();
        cout << "\n";

        for (int r = 0; r < H; r++) {
            bool hasDollar[60] = {};
            for (int i = 0; i < N; i++) {
                int dr = (startPos[i][0] + f * 2) % H;
                int dc = (startPos[i][1] + f * 3) % W;
                if (dr == TITLE_ROW && dc >= tStart - 1 && dc <= tEnd) continue;
                if (dr == r && dc >= 0 && dc < W) hasDollar[dc] = true;
            }

            string line = "          ";
            for (int c = 0; c < W; c++) {
                if (r == TITLE_ROW && c >= tStart && c < tEnd) {
                    if (c == tStart) { line += YELLOW; line += BOLD; }
                    line += title[c - tStart];
                    if (c == tEnd - 1) line += RESET;
                } else if (hasDollar[c]) {
                    line += GREEN;
                    line += '$';
                    line += RESET;
                } else {
                    line += ' ';
                }
            }
            cout << line << "\n";
        }
        sleepMs(300);
    }
}

void playCurrencyAnimation() {
    auto sleepMs = [](int ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    };

    const string sym[] = { "$", "\xE2\x82\xAC", "\xC2\xA5", "\xC2\xA3", "\xE2\x82\xB4" };
    const string clr[] = { GREEN, CYAN, RED, YELLOW, BLUE };

    // Frame 1a: few symbols appear
    clearScreen();
    cout << "\n\n"
         << "          " << clr[0] << sym[0] << RESET << "\n"
         << "\n"
         << "                          " << clr[1] << sym[1] << RESET << "\n"
         << "\n"
         << "\n"
         << "              " << clr[2] << sym[2] << RESET << "\n"
         << "\n\n" << RESET;
    sleepMs(300);

    // Frame 1b: more symbols scattered
    clearScreen();
    cout << "\n\n"
         << "          " << clr[0] << sym[0] << RESET << "                       " << clr[3] << sym[3] << RESET << "\n"
         << "\n"
         << "                          " << clr[1] << sym[1] << RESET << "\n"
         << "     " << clr[4] << sym[4] << RESET << "\n"
         << "                                      " << clr[0] << sym[0] << RESET << "\n"
         << "              " << clr[2] << sym[2] << RESET << "\n"
         << "                             " << clr[4] << sym[4] << RESET << "\n"
         << "\n" << RESET;
    sleepMs(300);

    // Frame 1c: all symbols dense
    clearScreen();
    cout << "\n\n"
         << "     " << clr[4] << sym[4] << RESET << "    " << clr[0] << sym[0] << RESET << "          " << clr[1] << sym[1] << RESET << "       " << clr[3] << sym[3] << RESET << "    " << clr[2] << sym[2] << RESET << "\n"
         << "                 " << clr[3] << sym[3] << RESET << "                   " << clr[4] << sym[4] << RESET << "\n"
         << "          " << clr[1] << sym[1] << RESET << "            " << clr[0] << sym[0] << RESET << "\n"
         << "     " << clr[0] << sym[0] << RESET << "       " << clr[2] << sym[2] << RESET << "       " << clr[4] << sym[4] << RESET << "       " << clr[1] << sym[1] << RESET << "    " << clr[3] << sym[3] << RESET << "\n"
         << "                          " << clr[3] << sym[3] << RESET << "\n"
         << "        " << clr[2] << sym[2] << RESET << "    " << clr[4] << sym[4] << RESET << "                  " << clr[0] << sym[0] << RESET << "\n"
         << "                    " << clr[1] << sym[1] << RESET << "          " << clr[2] << sym[2] << RESET << "\n"
         << "\n" << RESET;
    sleepMs(350);

    // Frame 2: full board with title
    clearScreen();
    cout << "\n"
         << "         " << YELLOW << "+---------------------------------+" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "   " << clr[0] << sym[0] << RESET << "     " << clr[1] << sym[1] << RESET << "     " << clr[4] << sym[4] << RESET << "     " << clr[3] << sym[3] << RESET << "     " << clr[2] << sym[2] << RESET << "     " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "       " << GREEN << "1" << RESET << " " << clr[0] << sym[0] << RESET << "  =  " << CYAN << "43.9" << RESET << " " << clr[4] << sym[4] << RESET << "            " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "       " << GREEN << "1" << RESET << " " << clr[1] << sym[1] << RESET << "  =  " << CYAN << "51.0" << RESET << " " << clr[4] << sym[4] << RESET << "            " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "   " << clr[4] << sym[4] << RESET << "     " << clr[2] << sym[2] << RESET << "     " << clr[0] << sym[0] << RESET << "     " << clr[1] << sym[1] << RESET << "     " << clr[3] << sym[3] << RESET << "     " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "+---------------------------------+" << RESET << "\n"
         << "\n"
         << "          " << CYAN << BOLD << "E X C H A N G E   R A T E S" << RESET << "\n"
         << RESET;
    sleepMs(900);
}

void playSettingsAnimation() {
    auto sleepMs = [](int ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    };

    // Frame 1: Small gear appears
    clearScreen();
    cout << "\n\n\n"
         << "                      " << YELLOW << "  ***  " << RESET << "\n"
         << "                      " << YELLOW << " *   * " << RESET << "\n"
         << "                      " << YELLOW << "  ***  " << RESET << "\n"
         << "\n\n\n" << RESET;
    sleepMs(600);

    // Frame 2: Full gear with language labels around it
    clearScreen();
    cout << "\n\n"
         << "           " << BLUE << "[" << GREEN << "UA" << BLUE << "]" << RESET << "                " << BLUE << "[" << GREEN << "EN" << BLUE << "]" << RESET << "\n"
         << "\n"
         << "                  " << YELLOW << "*********" << RESET << "\n"
         << "                  " << YELLOW << "**" << CYAN << BOLD << " (@) " << RESET << YELLOW << "**" << RESET << "\n"
         << "                  " << YELLOW << "*********" << RESET << "\n"
         << "\n"
         << "           " << BLUE << "[" << GREEN << "EN" << BLUE << "]" << RESET << "                " << BLUE << "[" << GREEN << "UA" << BLUE << "]" << RESET << "\n"
         << "\n" << RESET;
    sleepMs(600);

    // Frame 3: Final frame with title
    clearScreen();
    cout << "\n"
         << "         " << YELLOW << "+---------------------------------+" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "                                 " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "    " << BLUE << "[" << GREEN << "UA" << BLUE << "]" << RESET << "    " << YELLOW << "*********" << RESET << "    " << BLUE << "[" << GREEN << "EN" << BLUE << "]" << RESET << "    " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "            " << YELLOW << "**" << CYAN << BOLD << " (@) " << RESET << YELLOW << "**" << RESET << "            " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "     " << CYAN << "\xF0\x9F\x91\xA4" << RESET << "     " << YELLOW << "*********" << RESET << "     " << CYAN << "\xF0\x9F\x8C\x90" << RESET << "     " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "|" << RESET << "                                 " << YELLOW << "|" << RESET << "\n"
         << "         " << YELLOW << "+---------------------------------+" << RESET << "\n"
         << "\n"
         << "            " << CYAN << BOLD << "S  E  T  T  I  N  G  S" << RESET << "\n"
         << RESET;
    sleepMs(900);
}
