#include <Windows.h>
#include <string>
#include <random>
#include <stdint.h>
#include "Utils.h"

std::wstring GetRandomString(const std::size_t length)
{
	static std::wstring alpha_numeric_chars = L"0123456789";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> dis(0U, alpha_numeric_chars.size() - 1U);

	std::wstring random_string(L"", length);
	for (size_t i = 0; i < length; ++i)
	{
		random_string.at(i) = alpha_numeric_chars.at(dis(gen));
	}
	return random_string;
}