cmake_minimum_required(VERSION 4.2)
project(BankProject)

set(CMAKE_CXX_STANDARD 20)

add_executable(BankProject
        main.cpp
        AccountCatalog.h
        BaseAccount.h
        CreditAccount.h
        DebitAccount.h
        DepositAccount.h
        Headers.h
)