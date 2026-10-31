/*
 * ============================================================================
 *  CreditAccount.h — кредитный счёт
 * ============================================================================
 *
 *  СУТЬ ФАЙЛА
 *  Счёт, на котором можно уходить в минус в пределах кредитного лимита.
 *  Отрицательный баланс — это долг перед банком. Лимит и процентную ставку
 *  Bank копирует из услуги каталога в момент открытия счёта.
 *
 *  ПОЧЕМУ ФАЙЛ ВАЖЕН
 *  Здесь виден главный полиморфный эффект иерархии: переопределённый
 *  getMinBalance() возвращает не 0, а -creditLimit. Поэтому обычный
 *  setBalance() из BaseAccount для кредитного счёта автоматически разрешает
 *  отрицательный баланс до лимита, а за его пределами бросает исключение.
 *  Bank при этом по-прежнему работает со всеми счетами одинаково.
 *
 *  ВАЖНО: при открытии счёта сначала вызывайте setCreditLimit(), а потом
 *  setBalance(), иначе отрицательный баланс не пройдёт проверку.
 *
 *  ФОРМАТ В ФАЙЛЕ (после общих полей BaseAccount):
 *      ... id_услуги кредитный_лимит ставка
 * ============================================================================
 */
#pragma once
#include "../Headers.h"

class CreditAccount : public BaseAccount {
private:
    double creditLimit{};  // Кредитный лимит: насколько можно уйти в минус (>= 0)
    double interestRate{}; // Процентная ставка по кредиту, % годовых (0–100)

protected:
    // Чтение своих полей из файла (вызывается из BaseAccount::read)
    void readExtra(std::istream& in) override {
        double limit, rate;
        if (!(in >> limit >> rate)) {
            in.clear(std::ios::failbit);
            throw std::runtime_error("Ошибка чтения данных кредитного счета");
        }
        setCreditLimit(limit);
        setInterestRate(rate);
    }

    // Запись своих полей в файл (вызывается из BaseAccount::write)
    void writeExtra(std::ostream& out) const override {
        out << ' ' << creditLimit << ' ' << interestRate;
    }

    // Вывод своих полей в консоль (вызывается из BaseAccount::print)
    void printExtra(std::ostream& out) const override {
        out << " | Limit " << creditLimit
            << " | Rate " << interestRate << "%"
            << " | Debt " << getDebt();
    }

public:
    CreditAccount() = default;
    ~CreditAccount() override = default;

    // Возвращает тип счёта — используется при сохранении / загрузке
    AccountType getType() const noexcept override {
        return AccountType::Credit;
    }

    // Нижний предел баланса кредитного счёта — минус кредитный лимит
    double getMinBalance() const noexcept override {
        return -creditLimit;
    }

    // Геттеры
    double getCreditLimit() const noexcept { return creditLimit; }
    double getInterestRate() const noexcept { return interestRate; }

    // Текущий долг: сколько банку должны (0, если баланс не отрицательный)
    double getDebt() const noexcept {
        return getBalance() < 0 ? -getBalance() : 0.0;
    }

    // Сеттеры с проверкой
    void setCreditLimit(double _limit) {
        if (_limit < 0) {
            throw std::invalid_argument("Кредитный лимит не может быть отрицательным!");
        }
        if (_limit < getDebt()) {
            throw std::invalid_argument("Новый лимит меньше текущего долга по счету!");
        }
        creditLimit = _limit;
    }

    void setInterestRate(double _rate) {
        if (_rate < 0 || _rate > 100) {
            throw std::invalid_argument("Процентная ставка должна быть в диапазоне от 0 до 100!");
        }
        interestRate = _rate;
    }
};