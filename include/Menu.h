#pragma once
#include "Headers.h"

class Menu {
private:
	Bank &bank; // меню работает с банком, который создал main

	std::string readLine(const std::string &) const;

	int readInt(const std::string &) const;

	double readDouble(const std::string &) const;

	AccountType readType() const;

	AccountCurrency readCurrency() const;

	void printMainMenu() const;

	void printServices(const std::vector<std::shared_ptr<BankService> > &) const;

	void printMoney(const std::string &, double) const;

	void catalogMenu();

	void bankMenu();

	void limitsMenu();

	void fileMenu();

	void findServices();

	void addService();

	void removeService();

	void openAccount();

	void closeAccount();

	void checkLimits();

	void saveData();

	void loadData();

	void interest();

public:
	Menu(Bank &_bank) : bank(_bank) {
	}

	~Menu() = default;

	void run(); // главный цикл меню
};

//  Ввод

// Читает строку целиком (с пробелами). Если ввод закончился, бросает исключение.
std::string Menu::readLine(const std::string &prompt) const {
	std::cout << prompt;
	std::string line;
	if (!std::getline(std::cin, line)) {
		throw std::runtime_error("Ввод прерван!");
	}
	// на Windows в конце строки может остаться '\r'
	if (!line.empty() && line.back() == '\r') {
		line.pop_back();
	}
	return line;
}

// Читает целое число. Буквы, пустая строка или лишние символы дают исключение.
int Menu::readInt(const std::string &prompt) const {
	while (true) {
		try {
			std::istringstream in(readLine(prompt));
			int value;
			if (!(in >> value) || !(in >> std::ws).eof()) {
				throw std::invalid_argument("Ожидалось целое число!");
			}
			return value; // Ввод успешен, возвращаем число и выходим
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << " Пожалуйста, повторите ввод.\n";
		}
	}
}

// Читает число с точкой (например 12.5)
double Menu::readDouble(const std::string &prompt) const {
	while (true) {
		try {
			std::istringstream in(readLine(prompt));
			double value;
			if (!(in >> value) || !(in >> std::ws).eof()) {
				throw std::invalid_argument("Ожидалось число с точкой (например, 12.5)!");
			}
			return value; // Ввод успешен, возвращаем число и выходим
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << " Пожалуйста, повторите ввод.\n";
		}
	}
}


AccountType Menu::readType() const {
	while (true) {
		try {
			int number = readInt("Тип счета (0 - Debit, 1 - Credit, 2 - Deposit): ");
			if (number < 0 || number > static_cast<int>(AccountType::Deposit)) {
				throw std::invalid_argument("Неизвестный тип счета! Введите число от 0 до 2.");
			}
			return static_cast<AccountType>(number);
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << "\n";
		}
	}
}

AccountCurrency Menu::readCurrency() const {
	while (true) {
		try {
			int number = readInt("Валюта (0 - BYN, 1 - USD, 2 - RUB, 3 - EUR, 4 - CNY): ");
			if (number < 0 || number > 4) {
				throw std::invalid_argument("Неизвестная валюта! Введите число от 0 до 4.");
			}
			return static_cast<AccountCurrency>(number);
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << "\n";
		}
	}
}


// ===========================================================================
//  Вывод

void Menu::printMainMenu() const {
	std::cout << "\n========== БАНК ==========\n"
			<< "1. Каталог услуг\n"
			<< "2. Банк (открытые счета)\n"
			<< "3. Проверки и ограничения\n"
			<< "4. Файловые операции\n"
			<< "5. Расчет процентов по депозиту (доп. функция)\n"
			<< "6. Выход\n";
}

void Menu::printServices(const std::vector<std::shared_ptr<BankService> > &services) const {
	if (services.empty()) {
		std::cout << "\nНичего не найдено\n";
		return;
	}
	std::cout << "\nНайдено услуг: " << services.size() << "\n";
	std::copy(services.begin(), services.end(), std::ostream_iterator<std::shared_ptr<BankService> >(std::cout, "\n"));
}

// Печатает "текст: сумма" с двумя знаками после запятой
void Menu::printMoney(const std::string &text, double value) const {
	auto oldFlags = std::cout.flags();
	auto oldPrecision = std::cout.precision();

	std::cout << text << std::fixed << std::setprecision(2) << value << "\n";

	std::cout.flags(oldFlags);
	std::cout.precision(oldPrecision);
}

// ===========================================================================
//  Главный цикл

void Menu::run() {
	while (true) {
		printMainMenu();
		try {
			int choice = readInt("Выбор: ");
			switch (choice) {
				case 1: catalogMenu();
					break;
				case 2: bankMenu();
					break;
				case 3: limitsMenu();
					break;
				case 4: fileMenu();
					break;
				case 5: interest();
					break;
				case 6:
					std::cout << "До свидания!\n";
					return;
				default:
					std::cout << "Нет такого пункта меню!\n";
			}
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << "\n";
			if (std::cin.eof()) return; // ввод закончился, продолжать нечем
		}
	}
}

// ===========================================================================
//  1. Каталог услуг
// ===========================================================================

void Menu::catalogMenu() {
	while (true) {
		std::cout << "\n--- Каталог услуг ---\n"
				<< "1. Показать все услуги\n"
				<< "2. Найти услугу по критериям\n"
				<< "3. Добавить услугу\n"
				<< "4. Удалить услугу\n"
				<< "0. Назад\n";
		try {
			int choice = readInt("Выбор: ");
			if (choice == 0) return;

			switch (choice) {
				case 1:
					std::cout << "\n";
					bank.getCatalog().print(std::cout);
					break;
				case 2: findServices();
					break;
				case 3: addService();
					break;
				case 4: removeService();
					break;
				default:
					std::cout << "Нет такого пункта меню!\n";
			}
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << "\n";
			if (std::cin.eof()) return;
		}
	}
}

void Menu::findServices() {
	std::cout << "Искать по: 1 - типу счета, 2 - валюте, 3 - процентной ставке\n";
	int choice = readInt("Выбор: ");

	switch (choice) {
		case 1:
			printServices(bank.getCatalog().findByType(readType()));
			break;
		case 2:
			printServices(bank.getCatalog().findByCurrency(readCurrency()));
			break;
		case 3: {
			double minRate = readDouble("Ставка от (%): ");
			double maxRate = readDouble("Ставка до (%): ");
			printServices(bank.getCatalog().findByRate(minRate, maxRate));
			break;
		}
		default:
			std::cout << "Нет такого критерия!\n";
	}
}

void Menu::addService() {
	std::shared_ptr<BankService> service = std::make_shared<BankService>();

	// Каждый шаг защищен изнутри методов ввода
	service->setID(readInt("ID услуги: "));
	service->setName(readLine("Название: "));
	service->setType(readType());
	service->setCurrency(readCurrency());
	service->setInterestRate(readDouble("Процентная ставка (%): "));
	service->setMonthlyFee(readDouble("Комиссия в месяц: "));
	service->setMinBalance(readDouble("Минимальная сумма для открытия счета: "));

	if (service->getType() == AccountType::Credit) {
		service->setCreditLimit(readDouble("Кредитный лимит: "));
	}
	if (service->getType() == AccountType::Deposit) {
		service->setTermMonths(readInt("Срок (месяцев): "));
	}

	// Финальная валидация услуги банком
	try {
		bank.addService(service);
		std::cout << "Услуга добавлена\n";
	} catch (const std::exception &e) {
		std::cout << "\n[Ошибка валидации]: " << e.what() << "\nУслуга не добавлена.\n";
	}
}

void Menu::removeService() {
	bank.removeService(readInt("ID услуги для удаления: "));
	std::cout << "Услуга удалена\n";
}

// ===========================================================================
//  2. Банк (открытые счета)
// ===========================================================================

void Menu::bankMenu() {
	while (true) {
		std::cout << "\n--- Банк ---\n"
				<< "1. Показать все счета\n"
				<< "2. Открыть счет по услуге из каталога\n"
				<< "3. Закрыть счет\n"
				<< "4. Сортировка по балансу\n"
				<< "5. Поиск по владельцу\n"
				<< "0. Назад\n";
		try {
			int choice = readInt("Выбор: ");
			if (choice == 0) return;

			switch (choice) {
				case 1:
					std::cout << "\nСчетов всего: " << bank.getSize()
							<< ", открытых: " << bank.getOpenSize() << "\n";
					bank.print(std::cout);
					break;
				case 2: openAccount();
					break;
				case 3: closeAccount();
					break;
				case 4:
					bank.sortBalance();
					std::cout << "\nСчета по возрастанию баланса:\n";
					bank.print(std::cout);
					break;
				case 5:
					bank.findOwner(readLine("Часть ФИО владельца: "));
					break;
				default:
					std::cout << "Нет такого пункта меню!\n";
			}
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << "\n";
			if (std::cin.eof()) return;
		}
	}
}

void Menu::openAccount() {
	std::cout << "\n";
	bank.getCatalog().print(std::cout); // Показываем доступные услуги

	std::string owner = readLine("\nФИО владельца: ");
	int serviceID = readInt("ID услуги из каталога: ");
	double startBalance = readDouble("Начальная сумма: ");

	// Бизнес-проверка самого банка (хватает ли денег для открытия по этой услуге)
	try {
		int id = bank.openAccount(owner, serviceID, startBalance);
		std::cout << "Счет открыт. ID счета: " << id << "\n" << bank.findById(id) << "\n";
	} catch (const std::exception &e) {
		std::cout << "\nОшибка банка: " << e.what() << "\nОперация отменена. Возврат в подменю.\n";
	}
}

void Menu::closeAccount() {
	int id = readInt("ID счета для закрытия: ");
	double payout = bank.closeAccount(id);
	printMoney("Счет закрыт. Выплачено клиенту: ", payout);
}

// ===========================================================================
//  3. Проверки и ограничения

void Menu::limitsMenu() {
	while (true) {
		std::cout << "\n--- Проверки и ограничения ---\n"
				<< "1. Проверить соответствие ограничениям\n"
				<< "2. Показать отчет о нарушениях\n"
				<< "0. Назад\n";
		try {
			int choice = readInt("Выбор: ");
			if (choice == 0) return;

			switch (choice) {
				case 1: checkLimits();
					break;
				case 2:
					std::cout << "\n";
					bank.printViolations(std::cout);
					break;
				default:
					std::cout << "Нет такого пункта меню!\n";
			}
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << "\n";
			if (std::cin.eof()) return;
		}
	}
}

void Menu::checkLimits() {
	std::cout << "\nОткрытых счетов: " << bank.getOpenSize()
			<< ", максимум: " << MAX_ACCOUNTS << "\n";

	if (bank.checkLimits()) {
		std::cout << "Ограничения соблюдены\n";
	} else {
		std::cout << "Ограничения НАРУШЕНЫ\n";
	}
}

// ===========================================================================
//  4. Файловые операции

void Menu::fileMenu() {
	while (true) {
		std::cout << "\n--- Файловые операции ---\n"
				<< "1. Сохранить каталог и счета в файлы\n"
				<< "2. Загрузить каталог и счета из файлов\n"
				<< "0. Назад\n";
		try {
			int choice = readInt("Выбор: ");
			if (choice == 0) return;

			switch (choice) {
				case 1: saveData();
					break;
				case 2: loadData();
					break;
				default:
					std::cout << "Нет такого пункта меню!\n";
			}
		} catch (const std::exception &e) {
			std::cout << "Ошибка: " << e.what() << "\n";
			if (std::cin.eof()) return;
		}
	}
}

void Menu::saveData() {
	std::ofstream catalogOut(CATALOG_FILE);
	std::ofstream accountsOut(ACCOUNTS_FILE);
	if (!catalogOut || !accountsOut) {
		throw std::runtime_error("Не удалось открыть файлы для записи!");
	}

	bank.save(catalogOut, accountsOut);
	std::cout << "Сохранено в " << CATALOG_FILE << " и " << ACCOUNTS_FILE << "\n";
}

void Menu::loadData() {
	std::ifstream catalogIn(CATALOG_FILE);
	std::ifstream accountsIn(ACCOUNTS_FILE);
	if (!catalogIn || !accountsIn) {
		throw std::runtime_error("Не удалось открыть файлы для чтения!");
	}

	bank.load(catalogIn, accountsIn); // при ошибке в файле банк остается прежним
	std::cout << "Загружено услуг: " << bank.getCatalog().size()
			<< ", счетов: " << bank.getSize() << "\n";
}

// ===========================================================================
//  5. Дополнительная функция: проценты по депозиту

void Menu::interest() {
	int id = readInt("ID депозитного счета: ");
	int months = readInt("За сколько месяцев посчитать проценты: ");

	double income = bank.calculateInterest(id, months);
	printMoney("Начисленные проценты: ", income);
}
