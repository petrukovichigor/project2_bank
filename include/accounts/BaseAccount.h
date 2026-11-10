#pragma once
#include "../Headers.h"

enum class AccountType {
	Debit,
	Credit,
	Deposit,
	Unknown
};

enum class AccountStatus {
	Active,
	Frozen,
	Closed
};

enum class AccountCurrency {
	BYN,
	USD,
	RUB,
	EUR,
	CNY
};

// Преобразование перечислений в текст (для вывода в консоль)
inline std::string toString(AccountType type) {
	switch (type) {
		case AccountType::Debit: return "Debit";
		case AccountType::Credit: return "Credit";
		case AccountType::Deposit: return "Deposit";
		default: return "Unknown";
	}
}

inline std::string toString(AccountStatus status) {
	switch (status) {
		case AccountStatus::Active: return "Active";
		case AccountStatus::Frozen: return "Frozen";
		case AccountStatus::Closed: return "Closed";
		default: return "Unknown";
	}
}

inline std::string toString(AccountCurrency currency) {
	switch (currency) {
		case AccountCurrency::BYN: return "BYN";
		case AccountCurrency::USD: return "USD";
		case AccountCurrency::RUB: return "RUB";
		case AccountCurrency::EUR: return "EUR";
		case AccountCurrency::CNY: return "CNY";
		default: return "Unknown";
	}
}

// Чтобы можно было писать: out << currency
inline std::ostream &operator<<(std::ostream &out, AccountType type) { return out << toString(type); }
inline std::ostream &operator<<(std::ostream &out, AccountStatus status) { return out << toString(status); }
inline std::ostream &operator<<(std::ostream &out, AccountCurrency currency) { return out << toString(currency); }

// Класс BaseAccount
class BaseAccount {
private:
	std::string ownerName{}; // ФИО владельца счета
	double balance{}; // Текущий баланс
	int accountID{}; // ID счета
	int serviceID{}; // ID услуги из ServiceCatalog, по которой открыт счет
	AccountCurrency currency{AccountCurrency::BYN}; // Валюта счета
	AccountStatus status{AccountStatus::Active}; // Статус счета

protected:
	// Сюда наследники кладут СВОИ поля. У базового класса своих дополнительных полей нет, поэтому методы пустые
	virtual void readExtra(std::istream &) {
	} // чтение из файла
	virtual void writeExtra(std::ostream &) const {
	} // запись в файл (начинать с пробела)
	virtual void printExtra(std::ostream &) const {
	} // вывод в консоль

public:
	virtual ~BaseAccount() = default;

	// Тип счёта задаётся самим классом-наследником, поэтому сеттера setType() нет.
	virtual AccountType getType() const noexcept = 0;

	// Нижний предел баланса. У дебетового и депозитного счёта это 0, кредитный счёт переопределяет метод и возвращает отрицательный лимит.
	virtual double getMinBalance() const noexcept { return 0.0; }

	// Геттеры
	std::string getOwnerName() const noexcept { return ownerName; }
	double getBalance() const noexcept { return balance; }
	int getAccountID() const noexcept { return accountID; }
	int getServiceID() const noexcept { return serviceID; }
	AccountCurrency getCurrency() const noexcept { return currency; }
	AccountStatus getStatus() const noexcept { return status; }

	// Сеттеры с проверкой данных
	void setOwnerName(const std::string &_name) {
		if (_name.empty()) {
			throw std::invalid_argument("Имя владельца счета не может быть пустым!");
		}
		// '_' в файле означает пробел, а переводы строк и табуляция сломали бы формат
		if (_name.find_first_of("_\t\r\n") != std::string::npos) {
			throw std::invalid_argument("Имя владельца не должно содержать символ '_' и переводы строк!");
		}
		ownerName = _name;
	}

	void setBalance(double _balance) {
		if (_balance < getMinBalance()) {
			throw std::invalid_argument("Баланс не может быть ниже допустимого минимума для этого типа счета!");
		}
		balance = _balance;
	}

	void setAccountID(int _id) {
		if (_id < 0) {
			throw std::invalid_argument("ID не может быть отрицательным!");
		}
		accountID = _id;
	}

	void setServiceID(int _id) {
		if (_id < 0) {
			throw std::invalid_argument("ID услуги не может быть отрицательным!");
		}
		serviceID = _id;
	}

	void setCurrency(AccountCurrency _currency) {
		currency = _currency;
	}

	void setStatus(AccountStatus _status) {
		status = _status;
	}

	// Чтение из файла
	// Читает поля счёта после типа (тип уже прочитал Bank::load)
	virtual std::istream &read(std::istream &in) {
		std::string name;
		double bal;
		int id, currencyNumber, statusNumber, service;

		// 1. читаем поля базового класса
		if (!(in >> id >> name >> bal >> currencyNumber >> statusNumber >> service)) {
			throw std::runtime_error("Ошибка чтения файла: неверная запись счета!");
		}

		// 2. номера валюты и статуса должны существовать в перечислениях
		if (currencyNumber < 0 || currencyNumber > 4) {
			throw std::runtime_error("Ошибка чтения файла: неизвестная валюта!");
		}
		if (statusNumber < 0 || statusNumber > 2) {
			throw std::runtime_error("Ошибка чтения файла: неизвестный статус счета!");
		}

		// 3. в файле ФИО записано через '_', возвращаем пробелы
		std::replace(name.begin(), name.end(), '_', ' ');

		// 4. наследник читает свои поля (например, кредитный лимит)
		readExtra(in);

		// 5. применяем значения через сеттеры (они ещё раз проверяют данные)
		setAccountID(id);
		setOwnerName(name);
		setServiceID(service);
		setCurrency(static_cast<AccountCurrency>(currencyNumber));
		setStatus(static_cast<AccountStatus>(statusNumber));
		setBalance(bal); // последним: проверка зависит от getMinBalance(),
		// а он у кредитного счёта зависит от прочитанного лимита
		return in;
	}

	// Запись в файл
	virtual std::ostream &write(std::ostream &out) const {
		auto oldFlags = out.flags(); // std::fixed и std::setprecision изменяют глобальное состояние потока.
		auto oldPrecision = out.precision(); // если не вернуть старые настройки в конце метода, то весь последующий вывод в программе будет форматироваться так же, что может сломать отображение данных в других частях программы

		std::string name = ownerName;
		std::replace(name.begin(), name.end(), ' ', '_'); // ФИО одним словом

		out << static_cast<int>(getType()) << ' '
				<< accountID << ' '
				<< name << ' '
				<< std::fixed << std::setprecision(2) << balance << ' ' // деньги с 2 знаками
				<< static_cast<int>(currency) << ' '
				<< static_cast<int>(status) << ' '
				<< serviceID;
		writeExtra(out); // поля наследника (тоже с 2 знаками для дробных)

		out.flags(oldFlags);
		out.precision(oldPrecision);
		return out;
	}

	// Вывод в консоль
	virtual std::ostream &print(std::ostream &out) const {
		auto oldFlags = out.flags();
		auto oldPrecision = out.precision();

		out << std::left << std::fixed << std::setprecision(2)
				<< "ID: " << std::setw(4) << accountID << " | "
				<< std::setw(7) << getType() << " | "
				<< currency << " | "
				<< std::setw(6) << status << " | "
				<< "Bal: " << std::right << std::setw(9) << balance << std::left << " | ";
		printExtra(out); // поля наследника (каждое заканчивается на |)
		out << ownerName;

		out.flags(oldFlags);
		out.precision(oldPrecision);
		return out;
	}
};

// Операторы потоков
inline std::ostream &operator<<(std::ostream &out, const BaseAccount &s) {
	return s.print(out);
}

// чтобы std::copy и ostream_iterator умели печатать shared_ptr<BaseAccount>
inline std::ostream &operator<<(std::ostream &out, const std::shared_ptr<BaseAccount> &ptr) {
	if (ptr) ptr->print(out);
	else out << "<null account>"; // защита от пустого указателя
	return out;
}

inline std::istream &operator>>(std::istream &in, BaseAccount &s) {
	return s.read(in);
}
