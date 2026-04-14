#pragma once
#include <string>
#include "types.h"

void clearScreen();
void waitUser();
double getValidDouble();
std::string getCurrentDate();
bool parseAndValidateDate(const std::string& input, std::string& standardizedDate);
std::string getValidDate(AppLanguage lang);
std::string sanitize(std::string s);
std::string progressBar(double current, double target, int width = 20);
