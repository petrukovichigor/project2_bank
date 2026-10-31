/*
 * ============================================================================
 *  Bank.h — собирательный класс «Банк»
 * ============================================================================
 *
 *  СУТЬ ФАЙЛА
 *  Банк объединяет всё приложение: он АГРЕГИРУЕТ все открытые счета клиентов
 *  (vector<shared_ptr<BaseAccount>>) и владеет каталогом услуг (ServiceCatalog).
 *  Через него клиент открывает и закрывает счета, а программа ищет и
 *  сортирует счета, проверяет ограничения и сохраняет данные в файлы.
 *
 *  ПОЧЕМУ ФАЙЛ ВАЖЕН
 *  1. Здесь связаны две части проекта: каталог («что банк предлагает») и
 *     счета («что реально открыто»). Счёт можно открыть только по услуге из
 *     каталога: openAccount берёт услугу по ID, по её типу создаёт нужный
 *     класс счёта (Debit / Credit / Deposit) и копирует в него условия.
 *  2. Здесь соблюдаются ограничения: число открытых счетов не больше
 *     MAX_ACCOUNTS, услугу нельзя удалить, пока по ней открыты счета.
 *  3. Bank работает со всеми счетами одинаково через указатель на BaseAccount
 *     (полиморфизм): сортировка по балансу и поиск по владельцу не зависят от
 *     типа счёта.
 *
 *  ЗАКРЫТЫЕ СЧЕТА
 *  closeAccount не удаляет счёт из банка, а переводит его в статус Closed
 *  (история остаётся). Закрытые счета не учитываются в лимите MAX_ACCOUNTS.
 *
 *  ПОРЯДОК ПОДКЛЮЧЕНИЯ
 *  Файл лежит в include/, рядом с Headers.h. В Headers.h его можно
 *  подключать последним.
 * ============================================================================
 */
#pragma once
#include "Headers.h"

class Bank {
private:
    std::vector<std::shared_ptr<BaseAccount>> accounts;
    ServiceCatalog catalog;

    int getNextID() const; // ID для нового счета: максимальный существующий + 1
public:
    static constexpr int MAX_ACCOUNTS = 20;       // максимум открытых счетов в банке
    static constexpr int FIRST_ACCOUNT_ID = 1001; // ID самого первого счета

    Bank() = default;
    ~Bank() = default;
    long getSize() const { return accounts.size(); } // всего счетов, включая закрытые
    long getOpenSize() const;                        // только открытые (не Closed)
    const ServiceCatalog& getCatalog() const { return catalog; }

    void addService(const std::shared_ptr<BankService>&);
    void removeService(int);
    int openAccount(const std::string&, int, double);
    double closeAccount(int);
    std::shared_ptr<BaseAccount> findById(int) const;
    void findOwner(const std::string&) const;
    double calculateInterest(int, int) const;
    bool checkLimits() const;
    void printViolations(std::ostream&) const;
    void print(std::ostream&) const;
    void sortName();
    void sortBalance();
    void save(std::ostream&, std::ostream&) const;
    void load(std::istream&, std::istream&);
};

int Bank::getNextID() const {
    if (accounts.empty()) return FIRST_ACCOUNT_ID;
    auto last = std::max_element(accounts.begin(), accounts.end(), [](std::shared_ptr<BaseAccount> a, std::shared_ptr<BaseAccount> b) { return a->getAccountID() < b->getAccountID(); });
    return (*last)->getAccountID() + 1;
}

long Bank::getOpenSize() const {
    return std::count_if(accounts.begin(), accounts.end(), [](std::shared_ptr<BaseAccount> a) { return a->getStatus() != AccountStatus::Closed; });
}

// Добавляет услугу в каталог банка
void Bank::addService(const std::shared_ptr<BankService>& service) {
    catalog.add(service);
}

// Удаляет услугу из каталога, если по ней нет открытых счетов
void Bank::removeService(int serviceID) {
    bool used = std::any_of(accounts.begin(), accounts.end(), [serviceID](std::shared_ptr<BaseAccount> a) { return a->getServiceID() == serviceID && a->getStatus() != AccountStatus::Closed; });

    if (used) {
        throw std::invalid_argument("Нельзя удалить услугу: по ней открыты счета!");
    }
    catalog.remove(serviceID);
}

// Открывает счет клиенту по услуге из каталога. Возвращает ID нового счета.
// Все проверки идут ДО добавления в банк, поэтому при ошибке банк не меняется.
int Bank::openAccount(const std::string& owner, int serviceID, double startBalance) {
    if (getOpenSize() >= MAX_ACCOUNTS) {
        // Показываю экспериментальное исключение из класса Exceptions.h
        throw LimitExceededException("Превышен лимит открытых счетов в банке!");
    }

    std::shared_ptr<BankService> service = catalog.findById(serviceID);
    if (!service)
    {
        throw std::invalid_argument("Услуга с таким ID не найдена в каталоге!");
    }
    if (startBalance < service->getMinBalance())
    {
        throw std::invalid_argument("Начальная сумма меньше минимальной для этой услуги!");
    }

    // Тип услуги определяет класс счета. Условия услуги копируются в счет.
    // У кредитного счета лимит ставим ДО баланса (от него зависит минимум баланса).
    std::shared_ptr<BaseAccount> account;
    switch (service->getType())
    {
        case AccountType::Debit:
        {
            std::shared_ptr<DebitAccount> debit = std::make_shared<DebitAccount>();
            debit->setMonthlyFee(service->getMonthlyFee());
            account = debit;
            break;
        }
        case AccountType::Credit:
        {
            std::shared_ptr<CreditAccount> credit = std::make_shared<CreditAccount>();
            credit->setCreditLimit(service->getCreditLimit());
            credit->setInterestRate(service->getInterestRate());
            account = credit;
            break;
        }
        case AccountType::Deposit:
        {
            std::shared_ptr<DepositAccount> deposit = std::make_shared<DepositAccount>();
            deposit->setInterestRate(service->getInterestRate());
            deposit->setTermMonths(service->getTermMonths());
            account = deposit;
            break;
        }
        default:
            throw std::invalid_argument("У услуги неизвестный тип счета!");
    }

    account->setAccountID(getNextID());
    account->setOwnerName(owner);
    account->setServiceID(serviceID);
    account->setCurrency(service->getCurrency());
    account->setBalance(startBalance);

    accounts.push_back(account);
    return account->getAccountID();
}

// Закрывает счет: остаток выплачивается клиенту (возвращается), статус Closed.
// Счет с долгом закрыть нельзя.
double Bank::closeAccount(int accountID)
{
    std::shared_ptr<BaseAccount> account = findById(accountID);
    if (!account)
    {
        throw std::invalid_argument("Счет с таким ID не найден!");
    }
    if (account->getStatus() == AccountStatus::Closed)
    {
        throw std::invalid_argument("Счет уже закрыт!");
    }
    if (account->getBalance() < 0)
    {
        throw std::invalid_argument("Нельзя закрыть счет с долгом!");
    }

    double payout = account->getBalance();
    account->setBalance(0);
    account->setStatus(AccountStatus::Closed);
    return payout;
}

// Счет по ID или nullptr, если такого нет
std::shared_ptr<BaseAccount> Bank::findById(int accountID) const
{
    auto it = std::find_if(accounts.begin(), accounts.end(), [accountID](std::shared_ptr<BaseAccount> a) { return a->getAccountID() == accountID; });

    if (it == accounts.end()) return nullptr;
    return *it;
}

// Поиск счетов по владельцу: имя содержит введенный текст
void Bank::findOwner(const std::string& name) const
{
    std::vector<std::shared_ptr<BaseAccount>> found;
    std::copy_if(accounts.begin(), accounts.end(), std::back_inserter(found), [&name](std::shared_ptr<BaseAccount> a) { return a->getOwnerName().find(name) != std::string::npos; });

    if (found.empty())
    {
        std::cout << "\nNo accounts found!\n";
        return;
    }
    else
    {
        std::cout << "\nAccounts suitable for criteria:\n";
        std::copy(found.begin(), found.end(), std::ostream_iterator<std::shared_ptr<BaseAccount>>(std::cout, "\n"));
    }
}

// Дополнительная функция: проценты по депозитному счету за months месяцев
double Bank::calculateInterest(int accountID, int months) const
{
    std::shared_ptr<DepositAccount> deposit = std::dynamic_pointer_cast<DepositAccount>(findById(accountID));
    if (!deposit)
    {
        throw std::invalid_argument("Депозитный счет с таким ID не найден!");
    }
    return deposit->calculateInterest(months);
}

// Ограничение по общему числу счетов: открытых не больше MAX_ACCOUNTS
bool Bank::checkLimits() const
{
    return getOpenSize() <= MAX_ACCOUNTS;
}

// Отчет о нарушениях: превышен лимит, услуга счета не найдена в каталоге,
// тип или валюта счета не совпадают с услугой. Закрытые счета не проверяются.
void Bank::printViolations(std::ostream& out) const
{
    bool found = false;

    if (!checkLimits())
    {
        out << "Limit exceeded: " << getOpenSize() << " open accounts, maximum is " << MAX_ACCOUNTS << "\n";
        found = true;
    }

    for (const auto& account : accounts)
    {
        if (account->getStatus() == AccountStatus::Closed) continue;

        std::shared_ptr<BankService> service = catalog.findById(account->getServiceID());
        if (!service)
        {
            out << "Account " << account->getAccountID() << ": service " << account->getServiceID() << " not found in catalog\n";
            found = true;
            continue;
        }
        if (service->getType() != account->getType())
        {
            out << "Account " << account->getAccountID() << ": type does not match service " << service->getID() << "\n";
            found = true;
        }
        if (service->getCurrency() != account->getCurrency())
        {
            out << "Account " << account->getAccountID() << ": currency does not match service " << service->getID() << "\n";
            found = true;
        }
    }

    if (!found)
    {
        out << "No violations found\n";
    }
}

void Bank::print(std::ostream& out) const
{
    std::copy(accounts.begin(), accounts.end(), std::ostream_iterator<std::shared_ptr<BaseAccount>>(out, "\n"));
}

void Bank::sortName()
{
    std::sort(accounts.begin(), accounts.end(), [](std::shared_ptr<BaseAccount> a, std::shared_ptr<BaseAccount> b) { return a->getOwnerName() < b->getOwnerName(); });
}

void Bank::sortBalance()
{
    std::sort(accounts.begin(), accounts.end(), [](std::shared_ptr<BaseAccount> a, std::shared_ptr<BaseAccount> b) { return a->getBalance() < b->getBalance(); });
}

// Сохраняет каталог услуг и счета в два потока (два файла)
void Bank::save(std::ostream& catalogOut, std::ostream& accountsOut) const
{
    catalog.save(catalogOut);

    for (const auto& account : accounts)
    {
        account->write(accountsOut);
        accountsOut << '\n';
    }
}

// Загружает каталог и счета. Сначала читается каталог, потом счета.
// Читаем во временные объекты: если файл повреждён, банк остаётся прежним.
void Bank::load(std::istream& catalogIn, std::istream& accountsIn)
{
    ServiceCatalog newCatalog;
    newCatalog.load(catalogIn);

    std::vector<std::shared_ptr<BaseAccount>> newAccounts;
    int typeInt;
    while (accountsIn >> typeInt)
    {
        std::shared_ptr<BaseAccount> account;

        switch (static_cast<AccountType>(typeInt))
        {
            case AccountType::Debit:   account = std::make_shared<DebitAccount>(); break;
            case AccountType::Credit:  account = std::make_shared<CreditAccount>(); break;
            case AccountType::Deposit: account = std::make_shared<DepositAccount>(); break;
            default:
                throw std::runtime_error("Ошибка чтения файла: неизвестный тип счета!");
        }

        accountsIn >> *account;

        bool duplicate = std::any_of(newAccounts.begin(), newAccounts.end(), [&account](std::shared_ptr<BaseAccount> a) { return a->getAccountID() == account->getAccountID(); });
        if (duplicate)
        {
            throw std::runtime_error("Ошибка чтения файла: два счета с одинаковым ID!");
        }
        newAccounts.push_back(account);
    }

    // цикл мог закончиться не концом файла, а мусором вместо типа счета
    if (!accountsIn.eof())
    {
        throw std::runtime_error("Ошибка чтения файла: неверный формат записи счета!");
    }

    catalog = std::move(newCatalog);
    accounts = std::move(newAccounts);
}