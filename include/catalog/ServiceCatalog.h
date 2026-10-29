#pragma once
#include "../Headers.h"

class ServiceCatalog {
private:
	std::vector<std::shared_ptr<BaseAccount>> accounts;

public:
	void add(const std::shared_ptr<BaseAccount>& account) {
		if (account) accounts.push_back(account);
	}

	void print(std::ostream& out) const {
		out << ":\n";
		std::copy(accounts.begin(), accounts.end(),
			std::ostream_iterator<std::shared_ptr<BaseAccount>>(out, "\n"));
	}

	void sortByWeight() {
		std::sort(accounts.begin(), accounts.end(),
			[](std::shared_ptr<BaseAccount> & a, std::shared_ptr<BaseAccount> & b) { return a->getWeight() < b->getWeight(); });
	}

	void sortByPrice() {
		std::sort(accounts.begin(), accounts.end(),
			[](auto& a, auto& b) { return a->getPrice() < b->getPrice(); });
	}

	void sortBySugar() {
		std::sort(accounts.begin(), accounts.end(),
			[](auto& a, auto& b) { return a->getSugar() < b->getSugar(); });
	}

	void load(std::istream& in) {
		int typeInt;
		while (in >> typeInt) {
			AccountType type = static_cast<AccountType>(typeInt);
			std::shared_ptr<BaseAccount> account;

			switch (type) {
				case AccountType::Debit: account = std::make_shared<DebitAccount>(); break;
				case AccountType::Credit:   account = std::make_shared<CreditAccount>(); break;
				case AccountType::Deposit: account = std::make_shared<DepositAccount>(); break;
				default:
					std::cerr << "Ошибка: Неизвестный тип аккаунта (" << typeInt << ")\n";
					continue;
			}

			in >> *account;
			accounts.push_back(account);
		}
	}
};


