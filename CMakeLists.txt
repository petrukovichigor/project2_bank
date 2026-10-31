cmake_minimum_required(VERSION 4.2)
project(BankProject)

set(CMAKE_CXX_STANDARD 20)

add_executable(BankProject
        main.cpp
        include/accounts/BaseAccount.h
        include/accounts/CreditAccount.h
        include/accounts/DebitAccount.h
        include/accounts/DepositAccount.h
        include/Headers.h
        include/catalog/ServiceCatalog.h
        include/Bank.h
        include/Exceptions.h
        include/catalog/BankService.h
        menu.h
)