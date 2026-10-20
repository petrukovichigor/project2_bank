#pragma once
#include "Headers.h"

// Класс Marmalade — наследник BaseSweet
// Представляет мармеладную конфету с дополнительными характеристиками
class DepositAccount : public BaseAccount {
private:
	std::string fruit{}; // Основной фрукт (например, апельсин, яблоко)
	bool sugarCoated{}; // Обсыпка сахаром: true — есть, false — нет

public:
	DepositAccount() = default;

	~DepositAccount() override = default;

	// Возвращает тип конфеты — используется при сохранении/загрузке
	AccountType getType() const noexcept override {
		return AccountType::Deposit;
	}

	// Геттеры
	std::string getFruit() const noexcept { return fruit; }
	bool isSugarCoated() const noexcept { return sugarCoated; }

	// Сеттеры с проверкой
	void setFruit(const std::string &_fruit) {
		if (_fruit.empty()) throw std::invalid_argument("Фрукт не должен быть пустым");
		fruit = _fruit;
	}

	void setSugarCoated(bool _sugarCoated) {
		sugarCoated = _sugarCoated;
	}

	// Метод чтения данных из потока
	std::istream &read(std::istream &in) override {
		BaseAccount::read(in); // читаем name, weight, price, sugar

		std::string f;
		int coated;
		if (!(in >> f >> coated)) {
			in.clear(std::ios::failbit);
			throw std::runtime_error("Ошибка чтения данных Marmalade");
		}

		setFruit(f);
		setSugarCoated(coated != 0); // преобразуем int → bool

		return in;
	}

	// Метод вывода информации о конфете
	std::ostream &print(std::ostream &out) const noexcept override {
		BaseAccount::print(out); // выводим базовые поля
		out << " | fruit " << std::setw(10) << fruit
				<< " | sugarCoated " << (sugarCoated ? "yes" : "no");
		return out;
	}
};
