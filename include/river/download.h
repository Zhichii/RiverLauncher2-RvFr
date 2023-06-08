#pragma once
#include <river/defines.h>

int download(HWND win, HWND btn) {
	char versionId[256];
	lisMinecraftDownloads->getText(lisMinecraftDownloads->getSelIndex(), versionId);
	char tmpS[512];
	GetWindowTextA(*ediSettingsDir, tmpS, 512);
	strcatf(tmpS, "\\versions\\%s", versionId);
	mkdir(tmpS);
	MessageBoxA(win, tmpS, "Test", MB_OK | MB_ICONINFORMATION);
	int index = lisMinecraftDownloads->getSelIndex();
	Response resp = Get(versionManifest["versions"][index]["url"].asCString());
	char tmpS2[512];
	strcpyf(tmpS2, "%s\\%s.json", tmpS, versionId);
	Json::Value x;
	reader.parse(resp.GetText(), x);
	Json::StyledWriter sw;
	FILE* fp = fopen(tmpS2, "w");
	Json::String s = sw.write(x);
	fwrite(s.c_str(), 1, s.size(), fp);
	fclose(fp);
	strcpyf(tmpS, "Finished Download '%s'. ", versionId);
	MessageBoxA(win, tmpS, "Hint", MB_OK | MB_ICONINFORMATION);
	return 0;
}