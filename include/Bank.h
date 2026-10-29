#pragma once
#include "catalog/ServiceCatalog.h"
#include "Headers.h"


class Bank
{
private:
    std::vector<std::shared_ptr<BaseSweet>> present;
    double weight = 0; //������� ��� �������
    double cost = 0; //������� ���� �������
public:
    Bank() = default;
    ~Bank() = default;
    double getCost() const { return cost; }
    double getWeight() const { return weight; }
    void createByWeight(double, ServiceCatalog&);
    void createByCost(double, ServiceCatalog&);
    void print(std::ostream&);
    void findSweetSugar(double min, double max);
    long getSize() const { return present.size(); }
    void sortName();
    void sortWeight();
    void sortPrice();
    void sortSugar();
};

void Bank::findSweetSugar(double min, double max)
{
    std::sort(present.begin(), present.end(), [](std::shared_ptr<BaseAccount> a, std::shared_ptr<BaseAccount> b) { return a->getSugar() < b->getSugar(); });
    auto tempBegin = std::find_if(present.begin(), present.end(), [min](std::shared_ptr<BaseAccount> a) { return a->getSugar() >= min; });
    auto tempEnd = std::find_if(present.begin(), present.end(), [max](std::shared_ptr<BaseAccount> a) { return a->getSugar() > max; });

    if (tempBegin == present.end())
    {
        std::cout << "\nNot sweet!\n";
        return;
    }
    else
    {
        std::cout << "\nSweets suitable for criteria:\n";
        std::copy(tempBegin, tempEnd, std::ostream_iterator<std::shared_ptr<BaseAccount>>(std::cout, "\n"));
    }
}

void Bank::createByWeight(double weightMax, ServiceCatalog& sweet) //�������� ������� �� ����
{
    bool flag = true;
    Bank present;
    sweet.sortByWeight();

    while (weightMax > 0 && flag)
    {
        //for each ������ ��������� �� ������ ����������, ��� ���� begin() � end()
        //� ���������� ������� ������ ������� ���������� �������, ������� ��������� ��� ��������� (� ����� ������ �������� for_each)
        std::for_each(sweet.begin(), sweet.end(), [&weightMax, &flag, &present, &sweet](std::shared_ptr<BaseSweet> sweets)
            {
                if (sweets->getWeight() <= weightMax)
                {
                    weightMax -= sweets->getWeight();
                    present.cost += sweets->getPrice();
                    present.weight += sweets->getWeight();
                    present.present.push_back(sweets);
                }
                else if (weightMax < sweet.back()->getWeight())
                {
                    flag = false;
                }
            });
    }

    *this = present;
}

void Bank::createByCost(double costMax, ServiceCatalog& sweet)
{
    bool flag = true;
    Bank present;
    sweet.sortByWeight();

    while (costMax > 0 && flag)
    {
        std::for_each(sweet.begin(), sweet.end(), [&costMax, &flag, &present, &sweet](std::shared_ptr<BaseAccount> sweets)
            {
                if (sweets->getPrice() <= costMax)
                {
                    costMax -= sweets->getPrice();
                    present.cost += sweets->getPrice();
                    present.weight += sweets->getWeight();
                    present.present.push_back(sweets);
                }
                else if (costMax < sweet.back()->getPrice())
                {
                    flag = false;
                }
            });
    }

    *this = present;
}

void Bank::print(std::ostream& out)
{
    std::copy(present.begin(), present.end(), std::ostream_iterator<std::shared_ptr<BaseAccount>>(out, "\n"));
}

void Bank::sortName()
{
    std::sort(present.begin(), present.end(), [](std::shared_ptr<BaseAccount> a, std::shared_ptr<BaseAccount> b) { return a->getName() < b->getName(); });
}

void Bank::sortWeight()
{
    std::sort(present.begin(), present.end(), [](std::shared_ptr<BaseAccount> a, std::shared_ptr<BaseAccount> b) { return a->getWeight() < b->getWeight(); });
}

void Bank::sortPrice()
{
    std::sort(present.begin(), present.end(), [](std::shared_ptr<BaseAccount> a, std::shared_ptr<BaseAccount> b) { return a->getPrice() < b->getPrice(); });
}

void Bank::sortSugar()
{
    std::sort(present.begin(), present.end(), [](std::shared_ptr<BaseAccount> a, std::shared_ptr<BaseAccount> b) { return a->getSugar() < b->getSugar(); });
}