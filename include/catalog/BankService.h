#pragma once

class BankService {
	private:
	int id;
	std::string name;
	AccountType type;
	AccountCurrency currency;
	double interestRate;
	double monthlyFee;
	double minBalance;
	double creditLimit;
	int termMonths;
	public:
	BankService() = default;
	~BankService();
};