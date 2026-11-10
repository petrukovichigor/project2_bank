//константы проекта
#pragma once
#include <string>

//constexpr вшивает числа в код при компиляции (для скорости),
//а const используется для std::string, так как строки выделяют динамическую память при старте.
constexpr int MAX_ACCOUNTS = 20;                          // максимум открытых счетов в банке
constexpr int FIRST_ACCOUNT_ID = 1001;                    // ID самого первого счета
const std::string CATALOG_FILE = "data/catalog.txt";  // файл каталога услуг
const std::string ACCOUNTS_FILE = "data/bank.txt";    // файл счетов
