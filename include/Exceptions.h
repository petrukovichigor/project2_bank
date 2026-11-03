#pragma once
#include "Headers.h"

// Превышен лимит открытых счетов в банке
class LimitExceededException : public std::runtime_error {
public:
	explicit LimitExceededException(const std::string& message) : std::runtime_error(message) {}
};