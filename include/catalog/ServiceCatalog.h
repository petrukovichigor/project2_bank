/*
 * ============================================================================
 *  ServiceCatalog.h — каталог банковских услуг
 * ============================================================================
 *
 *  СУТЬ ФАЙЛА
 *  Каталог ВОЗМОЖНЫХ УСЛУГ банка: список всего, что банк предлагает клиентам
 *  (типы счетов с их условиями). Это справочник, в нём нет ни одного
 *  конкретного счёта клиента, только описания услуг (BankService).
 *  Класс умеет добавлять и удалять услуги, искать их по ID, типу, валюте и
 *  ставке, выводить список в консоль, сохранять в файл и загружать из файла.
 *
 *  ПОЧЕМУ ФАЙЛ ВАЖЕН
 *  1. Он разделяет два понятия, которые раньше были смешаны: «что банк
 *     предлагает» (ServiceCatalog) и «что реально открыто у клиентов» (Bank).
 *  2. Bank владеет одним ServiceCatalog и при открытии счёта достаёт из него
 *     услугу по ID (findById): без каталога счёт открыть нельзя.
 *  3. Каталог не знает про счета, поэтому запрет «нельзя удалить услугу, по
 *     которой открыты счета» проверяет Bank::removeService, а уже потом
 *     вызывает ServiceCatalog::remove.
 *
 *  ПОРЯДОК ПОДКЛЮЧЕНИЯ
 *  В Headers.h этот файл должен идти ПОСЛЕ BankService.h.
 *
 *  ФОРМАТ ФАЙЛА catalog.txt
 *  Одна услуга — одна строка (формат описан в BankService.h):
 *
 *      id название тип валюта ставка комиссия мин_сумма кредитный_лимит срок
 *
 *  Загрузка (load) полностью заменяет содержимое каталога. Если в файле
 *  ошибка, бросается исключение, а текущий каталог остаётся без изменений.
 * ============================================================================
 */

#pragma once
#include "../Headers.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

class ServiceCatalog {
private:
    std::vector<std::shared_ptr<BankService>> services;

public:
    // ---- Добавление и удаление ------------------------------------------------

    // Добавляет услугу. Бросает исключение, если услуга пустая, заполнена
    // неполностью (см. BankService::validate) или услуга с таким ID уже есть.
    void add(const std::shared_ptr<BankService>& service) {
        if (!service) {
            throw std::invalid_argument("Услуга не может быть пустой!");
        }
        service->validate();
        if (findById(service->getID())) {
            throw std::invalid_argument("Услуга с таким ID уже есть в каталоге!");
        }
        services.push_back(service);
    }

    // Удаляет услугу по ID. Бросает исключение, если такой услуги нет.
    void remove(int id) {
        auto it = std::find_if(services.begin(), services.end(),
            [id](const std::shared_ptr<BankService>& s) { return s->getID() == id; });
        if (it == services.end()) {
            throw std::invalid_argument("Услуга с таким ID не найдена!");
        }
        services.erase(it);
    }

    // ---- Поиск ------------------------------------------------------------------

    // Услуга по ID или nullptr, если такой нет
    std::shared_ptr<BankService> findById(int id) const {
        auto it = std::find_if(services.begin(), services.end(),
            [id](const std::shared_ptr<BankService>& s) { return s->getID() == id; });
        return it == services.end() ? nullptr : *it;
    }

    // Все услуги заданного типа счёта
    std::vector<std::shared_ptr<BankService>> findByType(AccountType type) const {
        std::vector<std::shared_ptr<BankService>> result;
        std::copy_if(services.begin(), services.end(), std::back_inserter(result),
            [type](const std::shared_ptr<BankService>& s) { return s->getType() == type; });
        return result;
    }

    // Все услуги в заданной валюте
    std::vector<std::shared_ptr<BankService>> findByCurrency(AccountCurrency currency) const {
        std::vector<std::shared_ptr<BankService>> result;
        std::copy_if(services.begin(), services.end(), std::back_inserter(result),
            [currency](const std::shared_ptr<BankService>& s) { return s->getCurrency() == currency; });
        return result;
    }

    // Все услуги со ставкой от minRate до maxRate включительно
    std::vector<std::shared_ptr<BankService>> findByRate(double minRate, double maxRate) const {
        if (minRate > maxRate) {
            throw std::invalid_argument("Нижняя граница ставки больше верхней!");
        }
        std::vector<std::shared_ptr<BankService>> result;
        std::copy_if(services.begin(), services.end(), std::back_inserter(result),
            [minRate, maxRate](const std::shared_ptr<BankService>& s) {
                return s->getInterestRate() >= minRate && s->getInterestRate() <= maxRate;
            });
        return result;
    }

    // ---- Размер -------------------------------------------------------------------
    std::size_t size() const noexcept { return services.size(); }
    bool empty() const noexcept { return services.empty(); }

    // ---- Вывод в консоль ----------------------------------------------------------
    void print(std::ostream& out) const {
        if (services.empty()) {
            out << "Каталог услуг пуст\n";
            return;
        }
        out << "Каталог услуг (" << services.size() << "):\n";
        std::copy(services.begin(), services.end(),
            std::ostream_iterator<std::shared_ptr<BankService>>(out, "\n"));
    }

    // ---- Файловые операции ----------------------------------------------------------

    // Записывает весь каталог: одна услуга на строку
    void save(std::ostream& out) const {
        for (const auto& service : services) {
            service->write(out);
            out << '\n';
        }
    }

    // Загружает каталог из файла и заменяет им текущее содержимое.
    // Читаем во временный каталог: если файл повреждён, исключение вылетит
    // раньше, чем мы тронем настоящий.
    void load(std::istream& in) {
        ServiceCatalog loaded;
        while (true) {
            in >> std::ws;              // пропускаем пробелы и пустые строки
            if (in.eof()) break;        // файл закончился
            auto service = std::make_shared<BankService>();
            in >> *service;
            loaded.add(service);        // add проверяет услугу и дубликаты ID
        }
        services = std::move(loaded.services);
    }
};