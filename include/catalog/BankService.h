/*
 * ============================================================================
 *  BankService.h — описание одной банковской услуги
 * ============================================================================
 *
 *  СУТЬ ФАЙЛА
 *  Здесь описана УСЛУГА, которую банк предлагает клиентам: «Дебетовый
 *  Standard», «Кредитный Лайт», «Депозит Выгодный 12 мес» и т.п. Это только
 *  описание условий (тип счёта, валюта, ставка, комиссия, лимит, срок).
 *  Владельца и баланса у услуги нет: это не счёт клиента, а шаблон, по
 *  которому счёт можно открыть.
 *
 *  ПОЧЕМУ ФАЙЛ ВАЖЕН
 *  1. Из услуг состоит ServiceCatalog — каталог того, что банк вообще
 *     предлагает. Именно это преподавательница назвала «каталогом возможных
 *     услуг».
 *  2. Bank::openAccount берёт услугу из каталога и по её условиям создаёт
 *     конкретный счёт: тип услуги определяет класс счёта (Debit / Credit /
 *     Deposit), а ставка, комиссия, лимит и срок копируются в этот счёт.
 *  3. Счёт хранит только ID услуги (BaseAccount::serviceID), поэтому файл не
 *     зависит от BaseAccount.h по классам, только по enum-ам AccountType и
 *     AccountCurrency.
 *  4. Все сеттеры проверяют данные и бросают исключения. Метод validate()
 *     проверяет согласованность услуги целиком (у кредита должен быть лимит,
 *     у депозита срок), его вызывают ServiceCatalog::add и read.
 *
 *  ПОРЯДОК ПОДКЛЮЧЕНИЯ
 *  В Headers.h этот файл должен идти ПОСЛЕ BaseAccount.h (нужны enum-ы
 *  AccountType и AccountCurrency) и ДО ServiceCatalog.h.
 *
 *  ФОРМАТ ЗАПИСИ УСЛУГИ В ФАЙЛЕ
 *  Одна запись — одна строка, поля разделены пробелами:
 *
 *      id название тип валюта ставка комиссия мин_сумма кредитный_лимит срок
 *
 *  Пример: 5 Кредитный_Лайт 1 0 18.50 0.00 0.00 1000.00 0
 *
 *  Название пишется одним словом: пробелы заменяются на '_' (при чтении
 *  превращаются обратно в пробелы). Тип и валюта хранятся числами (номер
 *  значения в enum class). Неиспользуемые поля равны 0 (например, у дебетовой
 *  услуги кредитный лимит и срок нулевые).
 * ============================================================================
 */

#pragma once
#include "../Headers.h"

class BankService {
private:
    int id{};                                        // ID услуги (уникален в каталоге)
    std::string name{};                              // Название услуги
    AccountType type{AccountType::Unknown};          // Тип счёта, который открывается по услуге
    AccountCurrency currency{AccountCurrency::BYN};  // Валюта счёта
    double interestRate{};                           // Процентная ставка, % годовых (0–100)
    double monthlyFee{};                             // Комиссия за обслуживание в месяц (>= 0)
    double minBalance{};                             // Минимальная сумма для открытия счёта (>= 0)
    double creditLimit{};                            // Кредитный лимит (только для кредитных услуг)
    int termMonths{};                                // Срок в месяцах (только для депозитных услуг)

public:
    BankService() = default;
    ~BankService() = default;

    // ---- Геттеры ------------------------------------------------------------
    int getID() const noexcept { return id; }
    std::string getName() const noexcept { return name; }
    AccountType getType() const noexcept { return type; }
    AccountCurrency getCurrency() const noexcept { return currency; }
    double getInterestRate() const noexcept { return interestRate; }
    double getMonthlyFee() const noexcept { return monthlyFee; }
    double getMinBalance() const noexcept { return minBalance; }
    double getCreditLimit() const noexcept { return creditLimit; }
    int getTermMonths() const noexcept { return termMonths; }

    // ---- Сеттеры с проверкой данных -----------------------------------------
    void setID(int _id) {
        if (_id < 0) {
            throw std::invalid_argument("ID услуги не может быть отрицательным!");
        }
        id = _id;
    }

    void setName(const std::string& _name) {
        if (_name.empty()) {
            throw std::invalid_argument("Название услуги не может быть пустым!");
        }
        // '_' в файле означает пробел, а переводы строк и табуляция сломали бы формат
        if (_name.find_first_of("_\t\r\n") != std::string::npos) {
            throw std::invalid_argument("Название услуги не должно содержать символ '_' и переводы строк!");
        }
        name = _name;
    }

    void setType(AccountType _type) {
        if (_type == AccountType::Unknown) {
            throw std::invalid_argument("Тип услуги не может быть Unknown!");
        }
        type = _type;
    }

    void setCurrency(AccountCurrency _currency) {
        currency = _currency;
    }

    void setInterestRate(double _rate) {
        if (_rate < 0 || _rate > 100) {
            throw std::invalid_argument("Процентная ставка должна быть в диапазоне от 0 до 100!");
        }
        interestRate = _rate;
    }

    void setMonthlyFee(double _fee) {
        if (_fee < 0) {
            throw std::invalid_argument("Комиссия не может быть отрицательной!");
        }
        monthlyFee = _fee;
    }

    void setMinBalance(double _minBalance) {
        if (_minBalance < 0) {
            throw std::invalid_argument("Минимальная сумма не может быть отрицательной!");
        }
        minBalance = _minBalance;
    }

    void setCreditLimit(double _limit) {
        if (_limit < 0) {
            throw std::invalid_argument("Кредитный лимит не может быть отрицательным!");
        }
        creditLimit = _limit;
    }

    void setTermMonths(int _months) {
        if (_months < 0) {
            throw std::invalid_argument("Срок не может быть отрицательным!");
        }
        termMonths = _months;
    }

    // ---- Проверка услуги целиком ------------------------------------------
    // Отдельные сеттеры не видят связей между полями, поэтому согласованность
    // проверяется здесь: вызывать после того, как все поля заданы.
    void validate() const {
        if (name.empty()) {
            throw std::invalid_argument("У услуги не задано название!");
        }
        if (type == AccountType::Unknown) {
            throw std::invalid_argument("У услуги не задан тип!");
        }
        if (type == AccountType::Credit && creditLimit <= 0) {
            throw std::invalid_argument("У кредитной услуги кредитный лимит должен быть больше нуля!");
        }
        if (type == AccountType::Deposit && termMonths <= 0) {
            throw std::invalid_argument("У депозитной услуги срок должен быть больше нуля!");
        }
    }

    // ---- Чтение из файла ----------------------------------------------------
    //     id название тип валюта ставка комиссия мин_сумма кредитный_лимит срок
    std::istream& read(std::istream& in) {
        std::string _name;
        int _id, typeNumber, currencyNumber, term;
        double rate, fee, minBal, limit;

        // 1. читаем все поля записи
        if (!(in >> _id >> _name >> typeNumber >> currencyNumber >> rate >> fee >> minBal >> limit >> term)) {
            in.clear(std::ios::failbit);
            throw std::runtime_error("Ошибка чтения файла: неверная запись услуги!");
        }

        // 2. номера типа и валюты должны существовать в перечислениях
        if (typeNumber < 0 || typeNumber > static_cast<int>(AccountType::Deposit)) {
            throw std::runtime_error("Ошибка чтения файла: неизвестный тип услуги!");
        }
        if (currencyNumber < 0 || currencyNumber > static_cast<int>(AccountCurrency::CNY)) {
            throw std::runtime_error("Ошибка чтения файла: неизвестная валюта!");
        }

        // 3. в файле название записано через '_', возвращаем пробелы
        std::replace(_name.begin(), _name.end(), '_', ' ');

        // 4. применяем значения через сеттеры (они проверяют данные)
        setID(_id);
        setName(_name);
        setType(static_cast<AccountType>(typeNumber));
        setCurrency(static_cast<AccountCurrency>(currencyNumber));
        setInterestRate(rate);
        setMonthlyFee(fee);
        setMinBalance(minBal);
        setCreditLimit(limit);
        setTermMonths(term);

        // 5. проверяем согласованность услуги целиком
        validate();
        return in;
    }

    // ---- Запись в файл ------------------------------------------------------
    // Без перевода строки в конце (его пишет ServiceCatalog::save).
    std::ostream& write(std::ostream& out) const {
        auto oldFlags = out.flags();          // запоминаем настройки потока,
        auto oldPrecision = out.precision();  // чтобы вернуть их обратно

        std::string fileName = name;
        std::replace(fileName.begin(), fileName.end(), ' ', '_');   // название одним словом

        out << id << ' '
            << fileName << ' '
            << static_cast<int>(type) << ' '
            << static_cast<int>(currency) << ' '
            << std::fixed << std::setprecision(2)   // деньги и проценты с 2 знаками
            << interestRate << ' '
            << monthlyFee << ' '
            << minBalance << ' '
            << creditLimit << ' '
            << termMonths;

        out.flags(oldFlags);
        out.precision(oldPrecision);
        return out;
    }

    // ---- Вывод в консоль ----------------------------------------------------
    // Одна строка, без перевода строки в конце.
    std::ostream& print(std::ostream& out) const {
        auto oldFlags = out.flags();
        auto oldPrecision = out.precision();

        out << std::left << std::fixed << std::setprecision(2)
            << "Service ID " << std::setw(4) << id
            << "| " << name << " "   // без setw: он считает байты, а кириллица в UTF-8 занимает 2 байта на букву
            << "| Type " << std::setw(8) << type
            << "| Currency " << std::setw(5) << currency
            << "| Rate " << interestRate << "% "
            << "| Fee " << monthlyFee << " "
            << "| Min " << minBalance << " "
            << "| Credit limit " << creditLimit << " "
            << "| Term " << termMonths << " mo";

        out.flags(oldFlags);
        out.precision(oldPrecision);
        return out;
    }
};

// ---------------------------------------------------------------------------
//  Операторы потоков
// ---------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const BankService& s) {
    return s.print(out);
}

// Нужен, чтобы std::copy и ostream_iterator умели печатать shared_ptr<BankService>
inline std::ostream& operator<<(std::ostream& out, const std::shared_ptr<BankService>& ptr) {
    if (ptr) ptr->print(out);
    else out << "<null service>"; // защита от пустого указателя
    return out;
}

inline std::istream& operator>>(std::istream& in, BankService& s) {
    return s.read(in);
}