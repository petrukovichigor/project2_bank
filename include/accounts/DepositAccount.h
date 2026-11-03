#pragma once
#include "../Headers.h"

class DepositAccount : public BaseAccount {
private:
    double interestRate{}; // Процентная ставка, % годовых (0–100)
    int termMonths{};      // Срок депозита в месяцах (> 0)

protected:
    // Чтение своих полей из файла (вызывается из BaseAccount::read)
    void readExtra(std::istream& in) override {
        double rate;
        int term;
        if (!(in >> rate >> term)) {
            in.clear(std::ios::failbit);
            throw std::runtime_error("Ошибка чтения данных депозитного счета");
        }
        setInterestRate(rate);
        setTermMonths(term);
    }

    // Запись своих полей в файл (вызывается из BaseAccount::write)
    void writeExtra(std::ostream& out) const override {
        out << ' ' << interestRate << ' ' << termMonths;
    }

    // Вывод своих полей в консоль (вызывается из BaseAccount::print)
    void printExtra(std::ostream& out) const override {
        out << " | Rate " << interestRate << "%"
            << " | Term " << termMonths << " mo";
    }

public:
    DepositAccount() = default;
    ~DepositAccount() override = default;

    // Возвращает тип счёта
    AccountType getType() const noexcept override {
        return AccountType::Deposit;
    }

    // Геттеры
    double getInterestRate() const noexcept { return interestRate; }
    int getTermMonths() const noexcept { return termMonths; }

    // Сеттеры с проверкой
    void setInterestRate(double _rate) {
        if (_rate < 0 || _rate > 100) {
            throw std::invalid_argument("Процентная ставка должна быть в диапазоне от 0 до 100!");
        }
        interestRate = _rate;
    }

    void setTermMonths(int _months) {
        if (_months <= 0) {
            throw std::invalid_argument("Срок депозита должен быть больше нуля!");
        }
        termMonths = _months;
    }

    // ДОП. ФУНКЦИЯ: начисленные проценты за заданное число месяцев.
    // TODO: добавить опцию капитализации
    // доход = баланс * ставка / 100 * месяцы / 12
    // Месяцев должно быть больше 0 и не больше срока депозита.
    double calculateInterest(int months) const {
        if (months <= 0) {
            throw std::invalid_argument("Число месяцев должно быть больше нуля!");
        }
        if (months > termMonths) {
            throw std::invalid_argument("Число месяцев превышает срок депозита!");
        }
        return getBalance() * interestRate / 100.0 * months / 12.0;
    }
};