#pragma once
#include <Windows.h> //Main lib for most WinAPI
#include <iostream>
#include "Error.h"
#define	MAXFILEPATHLEN	5000

BOOL EnableDebugAbilityWithChecks(void);
void RelaunchSelf(void);


BOOL EnableDebugAbilityWithChecks(void) {

	// Get the privilege value for SeDebugPrivilege
	// API - https://msdn.microsoft.com/en-us/library/windows/desktop/aa379180(v=vs.85).aspx
	// Privs - https://msdn.microsoft.com/en-us/library/windows/desktop/bb530716(v=vs.85).aspx
	LUID	privilegeLuid;
	if (!LookupPrivilegeValue(NULL, L"SeDebugPrivilege", &privilegeLuid))
	{
		Error("LookupPrivilegeValue()");
	}

	// Fill up the TOKEN_PRIVILEGES structure
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa379630(v=vs.85).aspx
	TOKEN_PRIVILEGES	tkPrivs;

	tkPrivs.PrivilegeCount = 1; // Only modify one privilege
	tkPrivs.Privileges[0].Luid = privilegeLuid; // specify the privilege to be modified i.e. SeDebugPrivilege
	tkPrivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // lets enable this privilege

	// Get current process handle - https://msdn.microsoft.com/en-us/library/windows/desktop/ms683179(v=vs.85).aspx
	// Get process token - https://msdn.microsoft.com/en-us/library/windows/desktop/aa379295(v=vs.85).aspx
	// Token access rights - https://msdn.microsoft.com/en-us/library/windows/desktop/aa374905(v=vs.85).aspx
	HANDLE	currentProcessHandle = GetCurrentProcess();
	HANDLE	processToken;

	// TOKEN_QUERY IS REQUIRED!!
	if (!OpenProcessToken(currentProcessHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &processToken)) {
		Error("OpenProcessToken()");
	}

	// MSDN sample: https://msdn.microsoft.com/en-us/library/aa390429(v=vs.85).aspx
	// Let's first get the structure size and check  if the privilege is available before enabling.

	DWORD structSize;
	GetTokenInformation(processToken, TokenPrivileges, NULL, 0, &structSize);


	// Get all available privileges and check if SeDebugPrivilege is available.
	DWORD structSize2;   // should come out of the API with same value as structSize2
	PTOKEN_PRIVILEGES processTokenPrivs;
	processTokenPrivs = (PTOKEN_PRIVILEGES)malloc(structSize);

	if (!GetTokenInformation(processToken, TokenPrivileges, processTokenPrivs, structSize, &structSize2)) {
		Error("GetTokenInformation()");
	}

	// Search for SeDebugPrivilege in list
	PLUID_AND_ATTRIBUTES runner;
	bool seDebugAvailable = false;

	for (DWORD x = 0; x < processTokenPrivs->PrivilegeCount; x++) {
		runner = &processTokenPrivs->Privileges[x];

		// RtlEqualLuid https://msdn.microsoft.com/en-us/library/windows/hardware/ff561842(v=vs.85).aspx
		// used in device drivers
		// LUID:  https://msdn.microsoft.com/en-us/library/windows/desktop/aa379261(v=vs.85).aspx
		if ((runner->Luid.LowPart == privilegeLuid.LowPart) && (runner->Luid.HighPart == privilegeLuid.HighPart)) {

			printf("SeDebugPrivilege available for enabling!\n");
			seDebugAvailable = true;
			break;
		}
	}

	if (!seDebugAvailable) {
		return FALSE;
	}

	// Enable  SeDebugPrivilege
	if (!AdjustTokenPrivileges(processToken, false, &tkPrivs, 0, NULL, NULL)) {
		Error("AdjustTokenPrivileges()");
	}
	return TRUE;
}

void RelaunchSelf(void) {
	SHELLEXECUTEINFO info;
	WCHAR fileName[MAXFILEPATHLEN];
	DWORD pathLen = MAXFILEPATHLEN;

	// GetModuleFilename returns path of current process executable
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197(v=vs.85).aspx
	GetModuleFileName(NULL, fileName, pathLen);

	// Structure details https://msdn.microsoft.com/en-us/library/windows/desktop/bb759784(v=vs.85).aspx
	// https://blogs.msdn.microsoft.com/vistacompatteam/2006/09/25/elevate-through-shellexecute/
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.fMask = SEE_MASK_DEFAULT;
	info.hwnd = NULL;
	info.lpVerb = L"runas";
	info.lpFile = fileName;
	info.lpParameters = NULL;
	info.lpDirectory = NULL;
	info.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&info);
}