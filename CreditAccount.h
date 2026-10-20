#pragma once
#include "Headers.h"

class CreditAccount : public BaseAccount {
private:
    std::string taste{}; // ���� (��������, ��������, ��������)
    std::string shape{}; // ����� (��������, �������, ��������)

public:
    CreditAccount() = default;
    ~CreditAccount() override = default;

    AccountType getType() const noexcept override {
        return AccountType::Credit; // ��� AccountType::Unknown, ���� ��������� ��������
    }

    std::string getTaste() const noexcept { return taste; }
    std::string getShape() const noexcept { return shape; }

    void setTaste(const std::string& _taste) {
        if (_taste.empty()) throw std::invalid_argument("���� �� ������ ���� ������");
        taste = _taste;
    }

    void setShape(const std::string& _shape) {
        if (_shape.empty()) throw std::invalid_argument("����� �� ������ ���� ������");
        shape = _shape;
    }

    std::istream& read(std::istream& in) override {
        BaseAccount::read(in);
        std::string t, s;
        if (!(in >> t >> s)) {
            in.clear(std::ios::failbit);
            throw std::runtime_error("������ ������ ������ Lollypop");
        }
        setTaste(t);
        setShape(s);
        return in;
    }

    std::ostream& print(std::ostream& out) const noexcept override {
        BaseAccount::print(out);
        out << " | taste " << std::setw(10) << taste
            << " | shape " << std::setw(10) << shape;
        return out;
    }
};

