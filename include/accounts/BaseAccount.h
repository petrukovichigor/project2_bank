/*
 *  BaseAccount.h — абстрактный базовый класс банковского счёта
 * ============================================================================
 *
 *  Здесь описан КОНКРЕТНЫЙ СЧЁТ КЛИЕНТА: владелец, баланс, ID, валюта, статус
 *  и идентификатор услуги, по которой счёт был открыт. Это не «услуга банка»
 *  (описание услуг лежит в BankService / ServiceCatalog), а именно реальный
 *  счёт, который хранится в классе Bank.
 *
 *  1. Это корень иерархии счетов: DebitAccount, CreditAccount и DepositAccount
 *     наследуются отсюда. Bank хранит их все как std::shared_ptr<BaseAccount>
 *     и работает с ними полиморфно (сортировка по балансу, поиск по владельцу,
 *     печать, сохранение), не зная, какой именно тип счёта перед ним.
 *  2. Класс абстрактный: чисто виртуальный getType() не даёт создать «просто
 *     счёт» — только дебетовый, кредитный или депозитный.
 *  3. Все сеттеры проверяют данные и бросают исключения.
 *  4. Здесь же задан формат строки счёта в файле (read / write), который
 *     используют Bank::save и Bank::load.
 *
 *  Каждое поле заканчивается символом ';', запись заканчивается переводом
 *  строки (его пишет Bank::save). Порядок полей:
 *
 *      (тип);id;владелец;баланс;валюта;статус;id услуги;
 *
 *  Пример: 2;17;Петров Пётр Петрович;1500.50;1;0;4;
 *
 *  Тип, валюта и статус хранятся числами (значение enum class). Тип читает
 *  Bank::load функцией readAccountType(), создаёт объект нужного класса, а
 *  остальные поля заполняет вызовом read() (operator>>).
 *
 * ============================================================================
 */

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

//  Преобразование перечислений в текст (для вывода в консоль)
inline std::string toString(AccountType type) {
    switch (type) {
        case AccountType::Debit:   return "Debit";
        case AccountType::Credit:  return "Credit";
        case AccountType::Deposit: return "Deposit";
        default:                   return "Unknown";
    }
}

inline std::string toString(AccountStatus status) {
    switch (status) {
        case AccountStatus::Active: return "Active";
        case AccountStatus::Frozen: return "Frozen";
        case AccountStatus::Closed: return "Closed";
        default:                    return "Unknown";
    }
}

inline std::string toString(AccountCurrency currency) {
    switch (currency) {
        case AccountCurrency::BYN: return "BYN";
        case AccountCurrency::USD: return "USD";
        case AccountCurrency::RUB: return "RUB";
        case AccountCurrency::EUR: return "EUR";
        case AccountCurrency::CNY: return "CNY";
        default:                   return "Unknown";
    }
}

// Чтобы можно было писать: out << std::setw(8) << currency
inline std::ostream& operator<<(std::ostream& out, AccountType type)         { return out << toString(type); }
inline std::ostream& operator<<(std::ostream& out, AccountStatus status)     { return out << toString(status); }
inline std::ostream& operator<<(std::ostream& out, AccountCurrency currency) { return out << toString(currency); }

//  Класс BaseAccount
class BaseAccount {
private:
    std::string ownerName{};   // ФИО владельца счета
    double balance{};          // Текущий баланс
    int accountID{};           // ID счета
    int serviceID{};           // ID услуги из ServiceCatalog, по которой открыт счет
    // std::string openDate{}; // TODO: дата открытия счета (сделать через <chrono>)
    AccountCurrency currency{AccountCurrency::BYN}; // Валюта счета
    AccountStatus   status{AccountStatus::Active};  // Статус счета

public:
    virtual ~BaseAccount() = default;

    // Тип счёта задаётся самим классом-наследником, поэтому сеттера setType() нет.
    virtual AccountType getType() const noexcept = 0;

    // Нижний предел баланса. У дебетового и депозитного счёта это 0, кредитный счёт переопределяет метод и возвращает отрицательный лимит.
    virtual double getMinBalance() const noexcept { return 0.0; }

    // Геттеры
    std::string getOwnerName() const noexcept { return ownerName; }
    double getBalance() const noexcept { return balance; }
    int getAccountID() const noexcept { return accountID; }
    int getServiceID() const noexcept { return serviceID; }
    AccountCurrency getCurrency() const noexcept { return currency; }
    AccountStatus getStatus() const noexcept { return status; }

    // Сеттеры с проверкой данных
    void setOwnerName(const std::string& _name) {
        if (_name.empty()) {
            throw std::invalid_argument("Имя владельца счета не может быть пустым!");
        }
        // символ ';' и перевод строки сломали бы формат файла
        if (_name.find_first_of(";\r\n") != std::string::npos) {
            throw std::invalid_argument("Имя владельца не должно содержать символ ';' и переводы строк!");
        }
        ownerName = _name;
    }

    void setBalance(double _balance) {
        if (_balance < getMinBalance()) {
            throw std::invalid_argument("Баланс не может быть ниже допустимого минимума для этого типа счета!");
        }
        balance = _balance;
    }

    void setAccountID(int _id) {
        if (_id < 0) {
            throw std::invalid_argument("ID не может быть отрицательным!");
        }
        accountID = _id;
    }

    void setServiceID(int _id) {
        if (_id < 0) {
            throw std::invalid_argument("ID услуги не может быть отрицательным!");
        }
        serviceID = _id;
    }

    void setCurrency(AccountCurrency _currency) {
        currency = _currency;
    }

    void setStatus(AccountStatus _status) {
        status = _status;
    }

    // ---- Чтение из файла ----------------------------------------------------
    // Читает поля счёта ПОСЛЕ типа (тип уже прочитал Bank::load):
    //     id;владелец;баланс;валюта;статус;id услуги;
    virtual std::istream& read(std::istream& in) {
        std::string idText, owner, balanceText, currencyText, statusText, serviceText;

        // читаем 6 полей, каждое до символа ';'
        if (!std::getline(in, idText, ';')       || !std::getline(in, owner, ';') ||
            !std::getline(in, balanceText, ';')  || !std::getline(in, currencyText, ';') ||
            !std::getline(in, statusText, ';')   || !std::getline(in, serviceText, ';')) {
            throw std::runtime_error("Ошибка чтения файла: неполная запись счета!");
        }

        // переводим текст в числа; если в файле не число — это ошибка файла
        int id, currencyNumber, statusNumber, service;
        double bal;
        try {
            id             = std::stoi(idText);
            bal            = std::stod(balanceText);
            currencyNumber = std::stoi(currencyText);
            statusNumber   = std::stoi(statusText);
            service        = std::stoi(serviceText);
        } catch (const std::logic_error&) {
            throw std::runtime_error("Ошибка чтения файла: неверный формат числа в записи счета!");
        }

        // номера валюты и статуса должны существовать в перечислениях
        if (currencyNumber < 0 || currencyNumber > static_cast<int>(AccountCurrency::CNY)) {
            throw std::runtime_error("Ошибка чтения файла: неизвестная валюта!");
        }
        if (statusNumber < 0 || statusNumber > static_cast<int>(AccountStatus::Closed)) {
            throw std::runtime_error("Ошибка чтения файла: неизвестный статус счета!");
        }

        // применяем значения через сеттеры (они ещё раз проверяют данные)
        setAccountID(id);
        setOwnerName(owner);
        setServiceID(service);
        setCurrency(static_cast<AccountCurrency>(currencyNumber));
        setStatus(static_cast<AccountStatus>(statusNumber));
        setBalance(bal);   // последним: проверка зависит от getMinBalance()
        return in;
    }

    // ---- Запись в файл ------------------------------------------------------
    // Пишет счёт вместе с типом, без перевода строки в конце:
    //     тип;id;владелец;баланс;валюта;статус;id услуги;
    virtual std::ostream& write(std::ostream& out) const {
        auto oldFlags = out.flags();          // запоминаем настройки потока,
        auto oldPrecision = out.precision();  // чтобы вернуть их обратно

        out << static_cast<int>(getType()) << ';'
            << accountID << ';'
            << ownerName << ';'
            << std::fixed << std::setprecision(2) << balance << ';'  // деньги с 2 знаками
            << static_cast<int>(currency) << ';'
            << static_cast<int>(status) << ';'
            << serviceID << ';';

        out.flags(oldFlags);
        out.precision(oldPrecision);
        return out;
    }

    // ---- Вывод в консоль ----------------------------------------------------
    // Одна строка, без перевода строки в конце.
    virtual std::ostream& print(std::ostream& out) const {
        auto oldFlags = out.flags();
        auto oldPrecision = out.precision();

        out << std::left << std::fixed << std::setprecision(2)
            << "Type " << std::setw(8) << getType()
            << "| Account ID " << std::setw(6) << accountID
            << "| Owner Name " << std::setw(24) << ownerName
            << "| Balance " << std::setw(12) << balance
            << "| Currency " << std::setw(5) << currency
            << "| Status " << std::setw(8) << status
            << "| Service ID " << serviceID;

        out.flags(oldFlags);
        out.precision(oldPrecision);
        return out;
    }
};

// ---------------------------------------------------------------------------
//  Операторы потоков
// ---------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const BaseAccount& s) {
    return s.print(out);
}

// Нужен, чтобы std::copy и ostream_iterator умели печатать shared_ptr<BaseAccount>
inline std::ostream& operator<<(std::ostream& out, const std::shared_ptr<BaseAccount>& ptr) {
    if (ptr) ptr->print(out);
    else out << "<null account>"; // защита от пустого указателя
    return out;
}

inline std::istream& operator>>(std::istream& in, BaseAccount& s) {
    return s.read(in);
}