#pragma once

#include <river/defines.h>

int initData() {
	DWORD exist;
	RegCreateKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\RvL", 0, NULL, 0, KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS | KEY_READ | KEY_SET_VALUE | KEY_WRITE, NULL, &hData, &exist);
	int t;
	DWORD dwType;

	dwType = REG_DWORD;
	t = 0;
	if (RegQueryValueEx(hData, L"SelectedLaunch", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "SelectedLaunch", REG_DWORD, &t, 4);
	t = 970;
	if (RegQueryValueEx(hData, L"WindowWidth", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "WindowWidth", REG_DWORD, &t, 4);
	t = 600;
	if (RegQueryValueEx(hData, L"WindowHeight", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "WindowHeight", REG_DWORD, &t, 4);
	
	dwType = REG_SZ;
	if (RegQueryValueEx(hData, L"MinecraftDirectory", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "MinecraftDirectory", REG_SZ, "\0", 1);
	if (RegQueryValueEx(hData, L"Users", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "Users", REG_SZ, "Player\0", 7);
	return 0;
}