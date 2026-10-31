// main.cpp — небольшой тестовый запуск (не меню!)
// Прогоняет основные сценарии: загрузка, вывод, открытие и закрытие счетов,
// проценты по депозиту, сортировка, поиск, проверки, ожидаемые ошибки, сохранение.
// Когда будет готов класс меню, main станет коротким: создать Bank и запустить меню.

#include "include/Headers.h"

int main() {
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);

	std::ifstream catalogFile("data/catalog.txt");
	std::ifstream accountsFile("data/bank.txt");
	if (!catalogFile || !accountsFile) {
		std::cerr << "Ошибка открытия catalog.txt или bank.txt\n";
		return 1;
	}

	Bank bank;

	try {
		// 1. Загрузка: сначала каталог услуг, потом счета
		bank.load(catalogFile, accountsFile);

		std::cout << "=== 1. Каталог услуг ===\n";
		bank.getCatalog().print(std::cout);

		std::cout << "\n=== 2. Счета (всего " << bank.getSize()
			<< ", открытых " << bank.getOpenSize() << ") ===\n";
		bank.print(std::cout);

		// 2. Ограничения
		std::cout << "\n=== 3. Проверка ограничений ===\n";
		std::cout << "Лимит соблюден: " << (bank.checkLimits() ? "да" : "нет") << "\n";
		bank.printViolations(std::cout);

		// 3. Открытие счетов по услугам из каталога (2 - дебет, 5 - кредит, 9 - депозит)
		std::cout << "\n=== 4. Открытие счетов ===\n";
		int debitID = bank.openAccount("Тестов Тест Тестович", 2, 150);
		int creditID = bank.openAccount("Тестов Тест Тестович", 5, 0);
		int depositID = bank.openAccount("Тестов Тест Тестович", 9, 2000);
		std::cout << "Открыты счета: " << debitID << ", " << creditID << ", " << depositID << "\n";
		std::cout << bank.findById(debitID) << "\n";
		std::cout << bank.findById(creditID) << "\n";
		std::cout << bank.findById(depositID) << "\n";

		// 4. Дополнительная функция: проценты по депозиту
		std::cout << "\n=== 5. Проценты по депозиту (счет " << depositID << ") ===\n";
		std::cout << "За 6 месяцев:  " << bank.calculateInterest(depositID, 6) << "\n";
		std::cout << "За 12 месяцев: " << bank.calculateInterest(depositID, 12) << "\n";

		// 5. Сортировка и поиск
		std::cout << "\n=== 6. Сортировка по балансу ===\n";
		bank.sortBalance();
		bank.print(std::cout);

		std::cout << "\n=== 7. Поиск по владельцу: \"Иванов\" ===";
		bank.findOwner("Иванов");

		// 6. Закрытие счета
		std::cout << "\n=== 8. Закрытие счета " << debitID << " ===\n";
		std::cout << "Выплачено клиенту: " << bank.closeAccount(debitID) << "\n";
		std::cout << "Открытых счетов: " << bank.getOpenSize() << "\n";

		// 7. Ошибки, которые ДОЛЖНЫ возникнуть (если ошибки нет, пишем об этом)
		std::cout << "\n=== 9. Ожидаемые ошибки ===\n";
		auto expectError = [](const std::string& title, auto action) {
			try {
				action();
				std::cout << title << ": ОШИБКА НЕ ВОЗНИКЛА!\n";
			}
			catch (const std::exception& e) {
				std::cout << title << ": " << e.what() << "\n";
			}
		};
		expectError("Сумма меньше минимальной", [&] { bank.openAccount("Тестов Тест", 2, 50); });
		expectError("Услуги нет в каталоге", [&] { bank.openAccount("Тестов Тест", 99, 100); });
		expectError("Пустое имя владельца", [&] { bank.openAccount("", 1, 100); });
		expectError("Счет уже закрыт", [&] { bank.closeAccount(debitID); });
		expectError("Закрытие счета с долгом", [&] { bank.closeAccount(1004); });
		expectError("Счета с таким ID нет", [&] { bank.closeAccount(424242); });
		expectError("Проценты по не депозитному счету", [&] { bank.calculateInterest(1001, 12); });
		expectError("Срок больше срока депозита", [&] { bank.calculateInterest(depositID, 13); });
		expectError("Удаление услуги с открытыми счетами", [&] { bank.removeService(5); });
		expectError("Удаление несуществующей услуги", [&] { bank.removeService(77); });

		// 8. Сохранение (в отдельные файлы, чтобы не затереть исходные данные)
		std::ofstream catalogOut("data/catalog_out.txt");
		std::ofstream accountsOut("data/bank_out.txt");
		if (!catalogOut || !accountsOut) {
			std::cerr << "Ошибка создания файлов сохранения\n";
			return 1;
		}
		bank.save(catalogOut, accountsOut);
		std::cout << "\n=== 10. Сохранено в data/catalog_out.txt и data/bank_out.txt ===\n";
		// ЭКСПЕРИМЕНТАЛЬНЫЙ БЛОК (только для демонстрации).
		// Показывает собственное исключение LimitExceededException из Exceptions.h:
		// открываем счета, пока банк не дойдет до лимита MAX_ACCOUNTS, и ловим именно его.
		// К основной логике программы не относится.
		std::cout << "\n=== 11. Эксперимент: собственное исключение ===\n";
		try {
			while (true) {
				bank.openAccount("Лимитов Лим", 1, 0);
			}
		}
		catch (const LimitExceededException& e) {
			std::cout << "Поймано LimitExceededException: " << e.what() << "\n";
			std::cout << "Открытых счетов: " << bank.getOpenSize() << " из " << Bank::MAX_ACCOUNTS << "\n";
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Ошибка: " << e.what() << "\n";
		return 1;
	}

	return 0;
}