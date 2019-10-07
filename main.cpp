#include <iostream>
#include <Windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <inttypes.h>

using namespace std;

int GetProcessID(const char* ExeName)
{
	PROCESSENTRY32 ProcEntry = { 0 };
	HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (!SnapShot)
		return NULL;

	ProcEntry.dwSize = sizeof(ProcEntry);

	if (!Process32First(SnapShot, &ProcEntry))
		return NULL;

	do
	{
		if (!strcmp(ProcEntry.szExeFile, ExeName))
		{
			CloseHandle(SnapShot);
			return ProcEntry.th32ProcessID;
		}
	} while (Process32Next(SnapShot, &ProcEntry));

	CloseHandle(SnapShot);
	return NULL;
}

DWORD_PTR GetProcessBaseAddress(int processID)
{
	DWORD_PTR   baseAddress = 0;
	HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	HMODULE* moduleArray;
	LPBYTE      moduleArrayBytes;
	DWORD       bytesRequired;

	if (processHandle)
	{
		if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
		{
			if (bytesRequired)
			{
				moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

				if (moduleArrayBytes)
				{
					unsigned int moduleCount;

					moduleCount = bytesRequired / sizeof(HMODULE);
					moduleArray = (HMODULE*)moduleArrayBytes;

					if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
					{
						baseAddress = (DWORD_PTR)moduleArray[0];
					}

					LocalFree(moduleArrayBytes);
				}
			}
		}

		CloseHandle(processHandle);
	}

	return baseAddress;
}
int main()
{

	int pid = GetProcessID("cubeworld.exe");
	HANDLE achandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (pid == NULL)
	{
		cout << "Cannot Obtain Process." << endl;
		Sleep(3000);
		exit(-1);
	}
	else
	{
		uint8_t BAddr[4];
		DWORD_PTR Base = GetProcessBaseAddress(pid);
		printf("[+] Base Address: 0x%llx\n", Base);
		DWORD_PTR WeaponStats1;
		ReadProcessMemory(achandle, (PVOID)(Base + 0x551A80), &WeaponStats1, sizeof(WeaponStats1), 0);
		DWORD_PTR WeaponStats2;
		ReadProcessMemory(achandle, (PVOID)(WeaponStats1 + 0x8), &WeaponStats2, sizeof(WeaponStats2), 0);
		DWORD_PTR WeaponStats3;
		ReadProcessMemory(achandle, (PVOID)(WeaponStats2 + 0x1B0), &WeaponStats3, sizeof(WeaponStats3), 0);
		DWORD_PTR WeaponStats4;
		ReadProcessMemory(achandle, (PVOID)(WeaponStats3 + 0x448), &WeaponStats4, sizeof(WeaponStats4), 0);
		//DWORD_PTR WeaponStats5;
		//ReadProcessMemory(achandle, (PVOID)(WeaponStats4 + 0x6D8), &WeaponStats5, sizeof(WeaponStats5), 0);
		DWORD_PTR WeapStats = WeaponStats4 + 0x6D8;

		printf("[+] WeaponStats Base: 0x%llx\n", WeapStats);

		int32_t Weapon;
		ReadProcessMemory(achandle, (PVOID)(WeapStats + 0x4), &Weapon, sizeof(Weapon), 0);
		printf("[+] WeaponStats Weapon: %i\n", Weapon);

		int32_t Rarity;
		ReadProcessMemory(achandle, (PVOID)(WeapStats + 0x14), &Rarity, sizeof(Rarity), 0);
		printf("[+] WeaponStats Rarity: %i\n", Rarity);

		DWORD_PTR Coins1;
		ReadProcessMemory(achandle, (PVOID)(Base + 0x551E80), &Coins1, sizeof(Coins1), 0);
		DWORD_PTR Coins2;
		ReadProcessMemory(achandle, (PVOID)(Coins1 + 0x2E0), &Coins2, sizeof(Coins2), 0);
		DWORD_PTR CoinsBase;
		ReadProcessMemory(achandle, (PVOID)(Coins2 + 0x1E8), &CoinsBase, sizeof(CoinsBase), 0);

		int32_t Coins;
		ReadProcessMemory(achandle, (PVOID)(CoinsBase + 0xBC), &Coins, sizeof(Coins), 0);
		printf("[+] Coins: %i\n", Coins);

		MessageBox(0, "Weapon IDs go from 1-24. View them on my github page: https://github.com/Hayden959/cubehax", "Weapon IDs", MB_OK);

		static bool once = true;

		while (true) {
			int32_t weapinput;
			int32_t rareinput;
			int32_t coinsinput;
			cout << "Type weapon ID >> ";
			cin >> weapinput;
			WriteProcessMemory(achandle, (LPVOID*)(WeapStats + 0x4), &weapinput, sizeof(weapinput), 0);
			cout << "Type weapon Rarity >> ";
			cin >> rareinput;
			WriteProcessMemory(achandle, (LPVOID*)(WeapStats + 0x14), &rareinput, sizeof(rareinput), 0);
			if (once) {
				cout << "Type Coins >> ";
				cin >> coinsinput;
				WriteProcessMemory(achandle, (LPVOID*)(CoinsBase + 0xBC), &coinsinput, sizeof(coinsinput), 0);
				once = false;
			}
		}
		Sleep(1);
	}

	return 0;
}
