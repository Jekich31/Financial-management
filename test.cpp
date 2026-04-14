#include "test.h"
#include "utils.h"
#include <termios.h>
#include <unistd.h>

int interactiveMenu(const string& header, const vector<string>& options) {
    int selected = 0;
    int numOptions = options.size();

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (true) {
        clearScreen();
        cout << header << "\n";
        cout << "=========================================\n";

        for (int i = 0; i < numOptions; ++i) {
            if (i == selected) {
                cout << " > \033[7m " << options[i] << " \033[0m\n";
            }
            else {
                cout << "   " << options[i] << "\n";
            }
        }
        cout.flush();

        unsigned char ch;
        read(STDIN_FILENO, &ch, 1);

        if (ch == 27) {
            unsigned char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 && seq[0] == '[') {
                if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                    if (seq[1] == 'A') {
                        selected = (selected - 1 + numOptions) % numOptions;
                    }
                    else if (seq[1] == 'B') {
                        selected = (selected + 1) % numOptions;
                    }
                }
            }
        }
        else if (ch == '\n' || ch == '\r') {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            tcflush(STDIN_FILENO, TCIFLUSH);
            cin.clear();
            return selected;
        }
    }
}