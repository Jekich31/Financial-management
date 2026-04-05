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


        int key = _getch();

        if (key == 224 || key == 0) {
            key = _getch();
            if (key == 72) { 
                selected = (selected - 1 + numOptions) % numOptions;
            }
            else if (key == 80) { 
                selected = (selected + 1) % numOptions;
            }
        }

        else if (key == 13) {
            return selected;
        }
    }
}