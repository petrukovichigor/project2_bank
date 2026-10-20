#pragma once
#include "Headers.h"

enum class AccountType {
    Debit,   // ���������� �������
    Credit,     // ����������� �������
    Deposit,   // ����������� �������
    Unknown      // ����������� ��� (�� ������ ������)
};

// ����������� ������� ����� ��� ���� ������
// ��������� ������������, ����������� � ���������� ������ � �������
class BaseAccount {
private:
    std::string name{};   // �������� �������
    double weight{};      // ��� � �������
    double price{};       // ���� � �������� ��������
    double sugar{};       // ���������� ������ (� ������� �� 100 � ��� ���������� �������� � ���������� � �����������)

public:
    // ����������� ���������� ��������� ��� ����������� �������� �������� ����� ��������� �� ������� �����
    virtual ~BaseAccount() = default;

    // ����� ����������� ����� � ���������� ��� �������
    // ���������� ��� ��������������� � ������ ������-����������
    virtual AccountType getType() const noexcept = 0;

    // ������� � ���������� ������ � ����� (������ ������)
    std::string getName() const noexcept { return name; }
    double getWeight() const noexcept { return weight; }
    double getPrice() const noexcept { return price; }
    double getSugar() const noexcept { return sugar; }

    // ������� � ������������� �������� � ��������� ������������
    // ���� ������ ����������� � ������������� ���������� std::invalid_argument

    void setName(const std::string& _name) {
        if (_name.empty()) throw std::invalid_argument("��� �� ������ ���� ������");
        name = _name;
    }

    void setWeight(double _weight) {
        if (_weight <= 0 || _weight > 10000) // �����������: �� 0 �� 10 ��
            throw std::invalid_argument("��� ������ ���� � ��������� (0; 10000]");
        weight = _weight;
    }

    void setPrice(double _price) {
        if (_price < 0 || _price > 1e9) // �����������: �������� ����
            throw std::invalid_argument("���� ������ ���� ��������������� � ��������");
        price = _price;
    }

    void setSugar(double _sugar) {
        if (_sugar < 0 || _sugar > 10000) // �����������: ��������������� ��������
            throw std::invalid_argument("���������� ������ ������ ���� ��������������� � ��������");
        sugar = _sugar;
    }

    // ����� ������ ������ �� ������ (��������, �� ����� ��� �������)
    // ���� ������ ����������� � ������������� ���������� std::runtime_error
    virtual std::istream& read(std::istream& in) {
        std::string n; double w, p, s;
        if (!(in >> n >> w >> p >> s)) {
            in.clear(std::ios::failbit);
            throw std::runtime_error("������ ������ ������ �������");
        }
        setName(n);
        setWeight(w);
        setPrice(p);
        setSugar(s);
        return in;
    }

    // ����� ������ ���������� � ������� � ����� (��������, �� ����� ��� � ����)
    // ����� ���� ������������ � ����������� ��� ������������ �������
    virtual std::ostream& print(std::ostream& out) const noexcept {
        out << std::left
            << "name " << std::setw(14) << name
            << "| weight " << std::setw(8) << weight
            << "| price " << std::setw(10) << price
            << "| sugar " << std::setw(8) << sugar;
        return out;
    }
};

// ���������� ��������� << ��� ������ ������� �� ������
// ��������� ������: std::cout << sweet;
inline std::ostream& operator<<(std::ostream& out, const BaseAccount& s) {
    return s.print(out);
}

// ���������� ��������� << ��� shared_ptr<BaseSweet>
// ��������� ������: std::cout << sweetPtr;
inline std::ostream& operator<<(std::ostream& out, const std::shared_ptr<BaseAccount>& ptr) {
    if (ptr) ptr->print(out);
    else out << "<null account>"; // ���� ��������� ������
    return out;
}

// ���������� ��������� >> ��� ������ ������� �� ������
// ��������� ������: std::cin >> sweet;
inline std::istream& operator>>(std::istream& in, BaseAccount& s) {
    return s.read(in);
}
