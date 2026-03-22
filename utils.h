#pragma once
#include <string>
#include "globals.h"

void clearScreen();
void waitUser();
double getValidDouble();
bool parseAndValidateDate(const std::string& input, std::string& standardizedDate);
std::string getValidDate(AppLanguage lang);
std::string sanitize(std::string s);
 