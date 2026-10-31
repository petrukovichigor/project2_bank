#include "include/Headers.h"

int main() {
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	std::ifstream file("data/input.txt");
	if (!file) {
		std::cerr << "Ошибка открытия input.txt\n";
		return 1;
	}

	std::vector<std::shared_ptr<BaseAccount>> accounts;

	int typeInt;
	while (file >> typeInt) {
		AccountType type = static_cast<AccountType>(typeInt);
		std::shared_ptr<BaseAccount> account;

		switch (type) {
			case AccountType::Debit: account = std::make_shared<DebitAccount>(); break;
			case AccountType::Credit: account = std::make_shared<CreditAccount>(); break;
			case AccountType::Deposit: account = std::make_shared<DepositAccount>(); break;
			default:
				std::cerr << "???" << typeInt << ")\n";
				continue;
		}


		file >> *account;
		accounts.push_back(account);

	}

	return 0;
}
