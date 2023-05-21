#pragma once

#include <river/defines.h>

int initData() {
	DWORD exist;
	RegCreateKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\RVL", 0, NULL, 0, KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS | KEY_READ | KEY_SET_VALUE | KEY_WRITE, NULL, &hData, &exist);
	int t = 0;
	if (exist == REG_CREATED_NEW_KEY) RegSetKeyValueA(hData, NULL, "SelectedLaunch", REG_DWORD, &t, 4);
	//RegGetValueA(HKEY_CURRENT_USER, "SOFTWARE\\RVL", );
	return 0;
}