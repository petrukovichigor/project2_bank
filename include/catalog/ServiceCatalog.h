#pragma once
#include "include/Headers.h"

class ServiceCatalog {
private:
	std::vector<std::shared_ptr<BankService>> services;

public:
	void add(const std::shared_ptr<BankService>& account) {
		if (account) services.push_back(account);
	}

	void print(std::ostream& out) const {
		out << ":\n";
		std::copy(services.begin(), services.end(),
			std::ostream_iterator<std::shared_ptr<BankService>>(out, "\n"));
	}

	void sortByWeight() {
		std::sort(services.begin(), services.end(),
			[](std::shared_ptr<BankService> & a, std::shared_ptr<BankService> & b) { return a->getWeight() < b->getWeight(); });
	}

	void sortByPrice() {
		std::sort(services.begin(), services.end(),
			[](auto& a, auto& b) { return a->getPrice() < b->getPrice(); });
	}

	void sortBySugar() {
		std::sort(services.begin(), services.end(),
			[](auto& a, auto& b) { return a->getSugar() < b->getSugar(); });
	}

	void load(std::istream& in) {
		int typeInt;
		while (in >> typeInt) {
			AccountType type = static_cast<AccountType>(typeInt);
			std::shared_ptr<BankService> service;

			switch (type) {
				case AccountType::Debit: service = std::make_shared<DebitAccount>(); break;
				case AccountType::Credit:   service = std::make_shared<CreditAccount>(); break;
				case AccountType::Deposit: service = std::make_shared<DepositAccount>(); break;
				default:
					std::cerr << "Ошибка: Неизвестный тип аккаунта (" << typeInt << ")\n";
					continue;
			}

			in >> *service;
			services.push_back(service);
		}
	}
};


