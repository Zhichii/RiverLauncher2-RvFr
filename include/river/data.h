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
	t = 0;
	if (RegQueryValueEx(hData, L"SelectedAccount", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "SelectedAccount", REG_DWORD, &t, 4);
	t = 0;
	if (RegQueryValueEx(hData, L"SelectedLang", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "SelectedLang", REG_DWORD, &t, 4);
	
	dwType = REG_SZ;
	wchar_t fileDir[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, fileDir, MAX_PATH);
	for (int i = MAX_PATH; i > 0; i--) {
		if (fileDir[i] == '\\') {
			fileDir[i] = 0;
			break;
		}
	}
	char cwd[MAX_PATH + 1] = { 0 };
	_getcwd(cwd, MAX_PATH);
	RegSetKeyValueA(hData, NULL, "LauncherCwd", REG_SZ, cwd, strlen(cwd) + 1);
	writeLog("Data", "%d", lstrlen(fileDir) + 1);
	RegSetKeyValue(hData, NULL, L"LauncherPath", REG_SZ, fileDir, lstrlen(fileDir)*2+1);
	if (RegQueryValueEx(hData, L"MinecraftDirectory", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "MinecraftDirectory", REG_SZ, "\0", 1);
	if (RegQueryValueEx(hData, L"Accounts", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "Accounts", REG_SZ, "[]\0", 3);
	if (RegQueryValueEx(hData, L"LatestKnown", NULL, &dwType, NULL, NULL) != ERROR_SUCCESS)
		RegSetKeyValueA(hData, NULL, "LatestKnown", REG_SZ, "\0", 1);

	langs[0] = Json::objectValue;
	langs[0]["id"] = "zh_cn";
	langs[0]["name"] = "中文";
	langs[0]["num"] = IDR_LANG1;
	langs[1] = Json::objectValue;
	langs[1]["id"] = "en_us";
	langs[1]["name"] = "English";
	langs[1]["num"] = IDR_LANG2;
	return 0;
}
