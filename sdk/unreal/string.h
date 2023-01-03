#pragma once

#include "../includes.h"

#include "tarray.h"
#include "../memory.h"

struct FString : public TArray<wchar_t>
{
	__forceinline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? (int32_t)std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	__forceinline bool IsValid() const
	{
		return Data != nullptr;
	}

	__forceinline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto realdata = Memory.Read<wchar_t*>((uintptr_t)Data);
		auto length = std::wcslen(realdata);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(realdata, realdata + length, '?', &str[0]);

		return str;
	}
};