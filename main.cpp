#include "include/Headers.h"

int main() {
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);

	Bank bank;

	// Загрузка данных при старте: сначала каталог услуг, потом счета
	std::ifstream catalogFile(CATALOG_FILE);
	std::ifstream accountsFile(ACCOUNTS_FILE);
	if (!catalogFile || !accountsFile) {
		std::cerr << "Ошибка открытия " << CATALOG_FILE << " или " << ACCOUNTS_FILE << "\n";
		return 1;
	}

	try {
		bank.load(catalogFile, accountsFile);
	} catch (const std::exception &e) {
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
