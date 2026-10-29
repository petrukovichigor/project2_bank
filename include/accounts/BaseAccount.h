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

class BaseAccount {
private:
    std::string ownerName{}; // ФИО владельца счета
    double balance{}; // Текущий баланс
    int accountID{}; // ID аккаунта
    // std::string openDate{}; СДЕЛАТЬ ЧЕРЕЗ ХРОНО
    AccountCurrency currency {AccountCurrency::BYN}; // Валюта счета (из enum AccountCurrency)
    AccountStatus status{AccountStatus::Active}; // Статус аккаунта (из enum AccountStatus)

public:

    virtual ~BaseAccount() = default;

    virtual AccountType getType() const noexcept = 0;
    std::string getOwnerName() const noexcept { return ownerName; }
    double getBalance() const noexcept { return balance; }
    int getAccountID() const noexcept { return accountID; }
    // std::string getOpenDate() const noexcept { return openDate; }
    AccountCurrency getCurrency() const noexcept { return currency; }
    AccountStatus getStatus() const noexcept { return status; }

    void setType(AccountType _type) {

    }

    void setOwnerName(const std::string& _name) {
        if (_name.empty()) {
            throw std::invalid_argument("Имя владельца счета не может быть пустым!");
        }
        ownerName = _name;
    }

    void setBalance(double _balance) {
        if (_balance < 0)
            throw std::invalid_argument("Баланс не может быть отрицательным!");
        balance = _balance;
    }

    void setAccountID(double _id) {
        if (_id < 0)
            throw std::invalid_argument("ID не может быть отрицательным!");
        accountID = _id;
    }

    // void setOpenDate(double _date) {
    //     if (_date < 0 || _date > 10000)
    //         throw std::invalid_argument("Неверный формат даты!");
    //     openDate = _date;
    // }

    void setCurrency(AccountCurrency _currency) {

    }

    void setStatus(AccountStatus _status) {

    }

    virtual std::istream& read(std::istream& in) {
        AccountType t; std::string n; double b; int id; AccountCurrency c; AccountStatus s;
        if (!(in >> t >> n >> b >> id >> c >> s)) {
            in.clear(std::ios::failbit);
            throw std::runtime_error("Ошибка чтения файла!");
        }
        setType(t);
        setOwnerName(n);
        setBalance(b);
        setAccountID(id);
        // setOpenDate(d);
        setCurrency(c);
        setStatus(s);
        return in;
    }

    virtual std::ostream& print(std::ostream& out) const noexcept {
        out << std::left
            << "Owner Name " << std::setw(14) << ownerName
            << "| Balance " << std::setw(8) << balance
            << "| Account ID " << std::setw(10) << accountID
            << "| Currency " << std::setw(8) << currency
            << "| Status " << std::setw(10) << status
        return out;
    }
};

inline std::ostream& operator<<(std::ostream& out, const BaseAccount& s) {
    return s.print(out);
}

inline std::ostream& operator<<(std::ostream& out, const std::shared_ptr<BaseAccount>& ptr) {
    if (ptr) ptr->print(out);
    else out << "<null account>"; // ���� ��������� ������
    return out;
}

inline std::istream& operator>>(std::istream& in, BaseAccount& s) {
    return s.read(in);
}
