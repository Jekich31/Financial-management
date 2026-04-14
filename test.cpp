#include "test.h"
#include "utils.h"
int interactiveMenu(const string& header, const vector<string>& options) {
    int selected = 0;
    int numOptions = options.size();

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


        int key = std::cin.get();

        if (key == 27) {
            int next1 = std::cin.get();
            if (next1 == '[') {
                int next2 = std::cin.get();
                if (next2 == 'A') {
                    selected = (selected - 1 + numOptions) % numOptions;
                }
                else if (next2 == 'B') {
                    selected = (selected + 1) % numOptions;
                }
            }
        }
        else if (key == '\n' || key == '\r' || key == 13) {
            return selected;
        }
    }
}