#include "menu.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <string>
#include "colors.h"
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

#ifndef _WIN32
void enableRawMode(termios& oldt) {
	termios newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void disableRawMode(termios& oldt) {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
#endif

int interactiveMenu(const string& header, const vector<string>& options) {
	int selected = 0;
	int numOptions = options.size();

#ifndef _WIN32
	termios oldt;
	enableRawMode(oldt);
#endif
	
	while (true) {
		clearScreen();

		// ====== HEADER ======
		cout << "==================================================================================\n";

	
		int width = 41;
		int pad = (width - (int)header.size()) / 2;
		if (pad < 0) pad = 0;

		cout << string(pad, ' ') << header << "\n";
		cout << "==================================================================================\n";

		// ====== OPTIONS ======
		for (int i = 0; i < numOptions; ++i) {
			string text = options[i];

			if (i == selected) {
				cout << "\n\t> \033[7m " << text << " \033[0m\n";
			}
			else {
				cout << "\n\t  " << text << "\n";
			}
		}
		cout << "\n==================================================================================\n";



#ifdef _WIN32
		int ch = _getch();

		if (ch == 224 || ch == 0) {
			int arrow = _getch();
			if (arrow == 72) {
				selected = (selected - 1 + numOptions) % numOptions;
			}
			else if (arrow == 80) {
				selected = (selected + 1) % numOptions;
			}
		}
		else if (ch == 13) {
			return selected;
		}

#else
		unsigned char ch;
		read(STDIN_FILENO, &ch, 1);

		if (ch == 27) {
			unsigned char seq[2];
			if (read(STDIN_FILENO, &seq[0], 1) && seq[0] == '[') {
				if (read(STDIN_FILENO, &seq[1], 1)) {
					if (seq[1] == 'A') {
						selected = (selected - 1 + numOptions) % numOptions;
					}
					else if (seq[1] == 'B') {
						selected = (selected + 1) % numOptions;
					}
				}
			}
		}
		else if (ch == '\n') {
			disableRawMode(oldt);
			return selected;
		}
#endif
	}
}