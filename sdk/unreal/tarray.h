#pragma once

#include "../includes.h"

template<class T>
struct TArray {
	friend struct FString;

public:
	inline TArray() {
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const {
		return Count;
	};

	inline T& operator[](int i) {
		return Data[i];
	};

	inline const T& operator[](int i) const {
		return Data[i];
	};

	inline bool IsValidIndex(int i) const {
		return i < Num();
	}

	inline T* GetData()
	{
		return Data;
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};