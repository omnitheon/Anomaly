//Anomaly.cpp

#include "Error.h"
#include "SID.h"
#include "Token.h"
#include "ElevatePriv.h"
#include "ProcessUtils.h"
#include "Files.h"
#include "Registry.h"

#pragma comment(lib, "User32.lib")
#pragma comment (lib, "wtsapi32.lib")
#pragma comment (lib, "ntdll") 
#pragma	comment(lib, "Advapi32")

int main()
{
	if (!EnableDebugAbilityWithChecks()) {
		printf("Could not get Debugging privilege...Relaunching Process as Elevated via ShellExec...\n");
		RelaunchSelf();
		ExitProcess(-1);
	}
	else
	{
		printf("Achieved Debugging privilege...\n");
	}

	dumpRunningProcessBaseline();
	ListFilesInDirectory(L"C:\\Windows\\");
	ListFilesInDirectory(L"C:\\Program Files\\");
	ListFilesInDirectory(L"C:\\Program Files (x86)\\");
	ListFilesInDirectory(L"C:\\ProgramData\\");
	ListFilesInDirectory(L"C:\\Users\\Default\\");
	ListFilesInDirectory(L"C:\\Users\\Public\\");
	ListFilesInDirectory(L"C:\\Users\\Administrator\\");
	ListFilesInDirectory(L"C:\\Users\\Guest\\");


	HKEY hTestKey;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\"), 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		QueryKey(hTestKey, L"HKEY_CURRENT_USER", L"SOFTWARE\\");
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\"), 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		QueryKey(hTestKey, L"HKEY_LOCAL_MACHINE", L"SOFTWARE\\");
	}

	RegCloseKey(hTestKey);
	return 0;
}
