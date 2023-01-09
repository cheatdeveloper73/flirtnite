#include "memory.h"

bool CMemory::Initialize()
{

	while (!GetProcessID())
	{

		WRAP_IF_DEBUG(
			std::cout << "Waiting for Fortnite's PID.\n";
		)

		Sleep(1000);

	}

	GamePID = GetProcessID();

	if (!GamePID)
	{
		WRAP_IF_DEBUG(
			std::cout << "Failed to get Fortnite's PID.\n";
		)

		ERROR_MESSAGE("Failed to initialize. (Code: 0x3)");

		return false; // failed to get the process id

	}

	#ifdef _RELEASE
	Sleep(5000);
	#endif

	while (!FindWindowA("UnrealWindow", NULL))
		Sleep(1000);

	Window = FindWindowA("UnrealWindow", NULL);

	if (!Window)
	{

		WRAP_IF_DEBUG(
			std::cout << "Failed to get Fortnite's window.\n";
		)

		ERROR_MESSAGE("Failed to initialize. (Code: 0x4)");

		return false;

	}

	if (!_HyperV->GetExtendProcCr3(GamePID))
	{
		WRAP_IF_DEBUG(
			std::cout << "Failed to extend the processes' CR3.\n";
		)

		ERROR_MESSAGE("Failed to initialize. (Code: 0x5)");

		return false;

	}

	BaseAddress = _HyperV->GetProccessBase();

	if (!BaseAddress)
	{
		WRAP_IF_DEBUG(
			std::cout << "Failed to get Fortnite's base address..\n";
		)

		ERROR_MESSAGE("Failed to initialize. (Code: 0x6)");

		return false;

	}
	
	WRAP_IF_DEBUG(
		std::cout << "Memory initialized.\n";
	)

	return true;

}

int GetProcessThreadNumByID(DWORD dwPID)
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	BOOL bRet = ::Process32First(hProcessSnap, &pe32);;
	while (bRet)
	{
		if (pe32.th32ProcessID == dwPID)
		{
			::CloseHandle(hProcessSnap);
			return pe32.cntThreads;
		}
		bRet = ::Process32Next(hProcessSnap, &pe32);
	}
	return 0;
}

DWORD CMemory::GetProcessID()
{
	DWORD dwRet = 0;
	DWORD dwThreadCountMax = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe32);
	do
	{
		if (wcscmp(pe32.szExeFile, L"FortniteClient-Win64-Shipping.exe") == 0)

		{
			DWORD dwTmpThreadCount = GetProcessThreadNumByID(pe32.th32ProcessID);

			if (dwTmpThreadCount > dwThreadCountMax)
			{
				dwThreadCountMax = dwTmpThreadCount;
				dwRet = pe32.th32ProcessID;
			}
		}
	} while (Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return dwRet;
}