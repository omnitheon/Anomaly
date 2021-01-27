#pragma once
#include <Windows.h> //Main lib for most WinAPI
#include <iostream>


void Error(const char* msg);

void Error(const char* msg) {
	//returns the error code that happened last when a win32api failed
	printf("Anomaly has exited......%s (ERROR CODE: %u)", msg, GetLastError());
	exit(1);
}