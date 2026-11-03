// main.cpp — точка входа.
// Создаем банк, загружаем в него каталог услуг и счета из файлов и
// передаем управление меню. Вся логика лежит в Bank, весь ввод/вывод в Menu.

#include "include/Headers.h"

int main() {
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);

	Bank bank;

	// Загрузка данных при старте: сначала каталог услуг, потом счета
	std::ifstream catalogFile(Menu::CATALOG_FILE);
	std::ifstream accountsFile(Menu::ACCOUNTS_FILE);
	if (!catalogFile || !accountsFile) {
		std::cerr << "Ошибка открытия " << Menu::CATALOG_FILE << " или " << Menu::ACCOUNTS_FILE << "\n";
		return 1;
	}

	try {
		bank.load(catalogFile, accountsFile);
	}
	catch (const std::exception& e) {
		std::cerr << "Ошибка загрузки данных: " << e.what() << "\n";
		return 1;
	}

	std::cout << "Загружено услуг: " << bank.getCatalog().size()
		<< ", счетов: " << bank.getSize() << "\n";

	// Запуск меню: работает, пока пользователь не выберет "Выход"
	Menu menu(bank);
	menu.run();

	return 0;
}