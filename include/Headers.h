#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <iterator>

#include "accounts/BaseAccount.h"
#include "accounts/DebitAccount.h"
#include "accounts/DepositAccount.h"
#include "accounts/CreditAccount.h"
#include "catalog/BankService.h"
#include "catalog/ServiceCatalog.h"
#include "Bank.h"

#include <windows.h>
