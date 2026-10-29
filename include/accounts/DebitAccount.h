#pragma once
#include "../Headers.h"
// Класс Chocolate — наследник BaseSweet
// Представляет шоколадную конфету с дополнительными характеристиками

class DebitAccount: public BaseAccount {
private:
    double cacao{};        // Содержание какао в процентах (0–100)
    std::string type{};    // Тип шоколада (например, "молочный", "горький")
    bool filling{};        // Наличие начинки: true — с начинкой, false — без

public:
    DebitAccount() = default;
    ~DebitAccount() override = default;

    // Возвращает тип конфеты — используется при сохранении/загрузке
    AccountType getType() const noexcept override {
        return AccountType::Debit;
    }


    // Геттеры для специфичных полей
    double getCacao() const noexcept { return cacao; }
    std::string getChocolateType() const noexcept { return type; }
    bool hasFilling() const noexcept { return filling; }

    // Сеттеры с проверкой корректности
    void setCacao(double _cacao) {
        if (_cacao < 0 || _cacao > 100)
            throw std::invalid_argument("Процент какао должен быть в диапазоне от 0 до 100");
        cacao = _cacao;
    }

    void setChocolateType(const std::string& _type) {
        if (_type.empty())
            throw std::invalid_argument("Тип шоколада не должен быть пустым");
        type = _type;
    }

    void setFilling(bool _filling) {
        filling = _filling;
    }

    // Метод чтения данных из потока
    // Сначала читаются базовые поля, затем специфичные
    std::istream& read(std::istream& in) override {
        BaseAccount::read(in); // читаем name, weight, price, sugar

        double c; std::string t; int f;
        if (!(in >> c >> t >> f)) {
            in.clear(std::ios::failbit);
            throw std::runtime_error("Ошибка чтения данных Chocolate");
        }

        setCacao(c);
        setChocolateType(t);
        setFilling(f != 0); // преобразуем int → bool

        return in;
    }

    // Метод вывода информации о конфете
    std::ostream& print(std::ostream& out) const noexcept override {
        BaseAccount::print(out); // выводим базовые поля
        out << " | cacao " << std::setw(6) << cacao
            << " | type " << std::setw(10) << type
            << " | filling " << (filling ? "yes" : "no");
        return out;
    }
};

