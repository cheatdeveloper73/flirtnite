#pragma once
#include <vector>
#include <array>
#include <string>

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif


struct CupArgs
{
	ULONG64 Argd[5];
};

EXTERN_C ULONG HyperCall(ULONG64 key, ULONG);
EXTERN_C ULONG CupCallEx(ULONG64 key, ULONG, CupArgs *command);
EXTERN_C ULONG64 __fastcall VmCallEx(ULONG64 Rcx, ULONG64 Rdx, ULONG64 R8, ULONG64 R9);
EXTERN_C NTSTATUS NtQuerySystemInformationEx(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

class HyperV
{
public:
	HyperV();
	ULONG InitHyperV();
	bool GetStatus();
	bool InitPageTableBase();
	bool GetExtendProcCr3(ULONG pid);
	ULONG64 GetCurrentProcCr3();
	ULONG64 Translate(ULONG64 Cr3, ULONG64 VirtAddr);
	ULONG ReadPhys(ULONG64 PhysAddr, ULONG64 Buffer, ULONG64 Size);
	ULONG WritePhys(ULONG64 PhysAddr, ULONG64 Buffer, ULONG64 Size);
	ULONG ExCopyVirt(ULONG64 SrcCr3, ULONG64 VirtSrc, ULONG64 DestCr3, ULONG64 VirtDest, ULONG64 Size);
	bool ReadMem(PVOID base, PVOID buffer, SIZE_T size);
	bool WriteMem(PVOID base, PVOID buffer, SIZE_T size);
	PVOID GetKeExport(const char* module_name, const char* export_name, bool rva = false);
	ULONG64 GetProcessModule(const wchar_t* Name);
	ULONG64 GetValGuardRegion();
	std::wstring ReadWideString(ULONG64 address);

	std::string ReadString(ULONG64 address);

	ULONG64 GetProccessBase()
	{
		return BaseAddress;
	}

	void SetGuardRegion(uintptr_t Va)
	{
		GuardReg = Va;
	}

	template <class T>
	T ReadClass64(uintptr_t Va)
	{
		T ret = {};
		if (ReadMem((PVOID)(Va), (PVOID)&ret, sizeof(T)))
			return ret;

		return {};
	}

	template <typename T>
	T ReadValue64(uintptr_t Va)
	{
		T ret{};
		if (ReadMem((PVOID)(Va), (PVOID)&ret, sizeof(T)))
			return ret;

		return ret;
	}

	template <typename T>
	std::vector<T> ReadVector(uintptr_t Va, int size)
	{
		std::vector<T> TempData{};
		TempData.resize(sizeof(T) * size);
		if (ReadMem((PVOID)(Va), &TempData[0], sizeof(T) * size))
			return TempData;

		return TempData;
	}

	template<typename T>
	T ReadChain(uintptr_t Va, std::vector<uint64_t> chain)
	{
		uint64_t current = Va;
		for (int i = 0; i < chain.size() - 1; i++)
		{
			current = ReadValue64<uint64_t>(current + chain[i]);
		}
		return ReadValue64<T>(current + chain[chain.size() - 1]);
	}

	template <typename T>
	bool WriteValue64(uintptr_t Va, T Buffer)
	{
		return WriteMem((LPVOID)Va, (PVOID)&Buffer, sizeof(T));
	}

	void ReadMemKe(void* dst, void* src, std::size_t size)
	{
		Syscall<decltype(&memcpy)>((PVOID)MemCopy, dst, src, size);
	}

	template <typename T>
	T ValReadGuarded(uintptr_t Src)
	{
		T buffer;
		if (!ReadMem((PVOID)(Src), &buffer, sizeof(T)))
			return 0;

		uintptr_t Val = GuardReg + (*(uintptr_t*)&buffer & 0xFFFFFF);
		return *(T*)&Val;
	}

	bool IsGuarded(uintptr_t Va)
	{
		uintptr_t result = Va & 0xFFFFFFF000000000;
		return result == 0x8000000000 || result == 0x10000000000;
	}

	uintptr_t GetWorldVol(uintptr_t Va)
	{
		uintptr_t UWorldOffset = 0;
		uintptr_t UWorldAddr = ReadValue64<uintptr_t>(Va + 0x60);
		if (UWorldAddr > 0x10000000000)
			UWorldOffset = UWorldAddr - 0x10000000000;
		else 
			UWorldOffset = UWorldAddr - 0x8000000000;
		
		return uintptr_t(Va + UWorldOffset);
	}

	template <typename T>
	T ReadVal(uintptr_t Va)
	{
		T buffer = ReadValue64< uintptr_t >(Va);
		if (IsGuarded((uintptr_t)buffer))
		{
			return ValReadGuarded<uintptr_t>(Va);
		}
		return buffer;
	}

	template <class T, class ... Ts>
	__forceinline std::invoke_result_t<T, Ts...> Syscall(void* addr, Ts ... args);
private:
	ULONG64 RandomKey;
	ULONG64 KernelCr3;
	ULONG64 CurentCr3;
	ULONG64 ExtendCr3;
	ULONG64 NtShutdownSystemPa;
	ULONG64 NtShutdownSystemVa;
	ULONG64 BaseAddress;
	ULONG64 ProcessPeb;
	ULONG64 MemCopy;
	ULONG64 PsLookupPeproc;
	ULONG64 ObDereferenceObject;
	ULONG64 ProcBaseProt;
	ULONG64 ProcPebProt;
	ULONG64 GuardReg;
};


#ifdef IS_HYPER_V
extern HyperV* m_pHyperV;
#endif
