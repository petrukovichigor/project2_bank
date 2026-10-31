/*
 * ============================================================================
 *  DebitAccount.h — дебетовый счёт
 * ============================================================================
 *
 *  Обычный расчётный счёт клиента: на нём лежат собственные деньги, уйти в
 *  минус нельзя (нижний предел баланса — 0, его задаёт BaseAccount).
 *  Своё поле у счёта одно: ежемесячная комиссия за обслуживание. Её значение
 *  Bank копирует из услуги каталога в момент открытия счёта.
 *
 *  Это один из трёх типов счетов в иерархии BaseAccount. Bank создаёт объект
 *  этого класса, когда клиент открывает счёт по услуге типа Debit, и хранит
 *  его как shared_ptr<BaseAccount>.
 *
 *  ФОРМАТ В ФАЙЛЕ (после общих полей BaseAccount):
 *      ... комиссия_в_месяц
 *
 * ============================================================================
 */

#pragma once
#include "../Headers.h"

class DebitAccount : public BaseAccount {
private:
    double monthlyFee{}; // Комиссия за обслуживание в месяц (>= 0)

protected:
    // Чтение своего поля из файла (вызывается из BaseAccount::read)
    void readExtra(std::istream& in) override {
        double fee;
        if (!(in >> fee)) {
            in.clear(std::ios::failbit);
            throw std::runtime_error("Ошибка чтения данных дебетового счета");
        }
        setMonthlyFee(fee);
    }

    // Запись своего поля в файл (вызывается из BaseAccount::write)
    void writeExtra(std::ostream& out) const override {
        out << ' ' << monthlyFee;
    }

    // Вывод своего поля в консоль (вызывается из BaseAccount::print)
    void printExtra(std::ostream& out) const override {
        out << " | Fee " << monthlyFee;
    }

public:
    DebitAccount() = default;
    ~DebitAccount() override = default;

    // Возвращает тип счёта
    AccountType getType() const noexcept override {
        return AccountType::Debit;
    }

    // Геттер
    double getMonthlyFee() const noexcept { return monthlyFee; }

    // Сеттер с проверкой
    void setMonthlyFee(double _fee) {
        if (_fee < 0) {
            throw std::invalid_argument("Комиссия не может быть отрицательной!");
        }
        monthlyFee = _fee;
    }

    // Списывает с баланса месячную комиссию. Если денег не хватает, setBalance бросит исключение и баланс не изменится
    void chargeMonthlyFee() {
        setBalance(getBalance() - monthlyFee);
    }
};