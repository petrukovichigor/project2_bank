#include "Headers.h"

int main() {
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	std::ifstream file("input.txt");
	if (!file) {
		std::cerr << "Ошибка открытия input.txt\n";
		return 1;
	}

	std::vector<std::shared_ptr<BaseAccount>> sweets;

	int typeInt;
	while (file >> typeInt) {
		AccountType type = static_cast<AccountType>(typeInt);
		std::shared_ptr<BaseAccount> account;

		switch (type) {
			case AccountType::Debit: account = std::make_shared<DebitAccount>(); break;
			case AccountType::Credit: account = std::make_shared<CreditAccount>(); break;
			case AccountType::Deposit: account = std::make_shared<DepositAccount>(); break;
			default:
				std::cerr << "��������������: ����������� ��� ������� (" << typeInt << ")\n";
				continue;
		}


		file >> *account;
		sweets.push_back(account);

	}

	std::cout << "������ ������:\n";
	std::copy(sweets.begin(), sweets.end(),
		std::ostream_iterator<std::shared_ptr<BaseAccount>>(std::cout, "\n"));

	return 0;
}
