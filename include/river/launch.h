#include <river/defines.h>

int launchInstance(const char* versionId, const char* dir, HWND edit) {
	
	// Prepare

	if (accounts.size() == 0) {
		MessageBox(edit, L"No accounts created! ", L"Prompt", MB_OK | MB_ICONINFORMATION);
	}
	char cwd[512];
	strcpy(cwd, dir);
	strcat(cwd, "\\");
	char fvJson[512]; // Full Version Json
	strcpy(fvJson, cwd);
	strcat(fvJson, "versions\\");
	strcat(fvJson, versionId);
	strcat(fvJson, "\\");
	strcat(fvJson, versionId);
	strcat(fvJson, ".json");

	ifstream file(fvJson);
	Json::Value versionInfo;
	reader.parse(file, versionInfo);
	int mark = 0;
	Json::Value available;
	Json::Value libraries = versionInfo["libraries"];
	Json::Value tmp;
	char theVersion[514];
	Json::Value versionLib;
	Json::Value libNameSp;
	char libDir[514];
	char libSFName[514];
	char tmpS[514];
	char* tmpC;
	char* tmpC2;
	int tmpI;

	// Libraries

	for (Json::Value i : libraries) {
		valueSplit(i["name"].asCString(), ":", &tmp);
		tmp[2] = "";
		join(tmp, theVersion, 512, ":");
		mark = 0;
		if (i["downloads"].isMember("artifact")) {
			versionLib = i["downloads"]["artifact"];
			mark = 1;
		}
		if (i["downloads"].isMember("classifiers")) {
			versionLib = i["downloads"]["classifiers"];
			mark = 1;
		}
		else if (mark == 0) {
			if (i.isMember("name")) {
				valueSplit(i["name"].asCString(), ":", &libNameSp);
				tmpC = (char*)malloc(strlen(libNameSp.asCString())+2);
				strcpy(tmpC, libNameSp.asCString());
				libNameSp[0] = tmpC;
				join(libNameSp, libDir, 512, "\\");
				libNameSp.resize(libNameSp.size() - 2);
				join(libNameSp, libSFName, 512, "-");
				strcat(libSFName, ".jar");
				memset(tmpS, 0, 512);
				strcat(tmpS, "libraries\\");
				strcat(tmpS, libDir);
				for (int i = 0; i < strlen(libDir); i++) {
					if (libDir[i] == '/') libDir[i] = '\\';
				}
				available[theVersion] = tmpS;
				free(tmpC);
			}
			continue;
		}
		if (i.isMember("rules")) {
			tmpI = 0;
			for (Json::Value l : i["rules"]) {
				if (strcmp(l["action"].asCString(), "allow") == 0) {
					if (l.isMember("os")) {
						if (strcmp(l["os"]["name"].asCString(), "windows") == 0) {
							tmpI = 1;
						}
					}
					else tmpI = 0;
				}
				if (strcmp(l["action"].asCString(), "disallow") == 0) {
					if (l.isMember("os")) {
						if (strcmp(l["os"]["name"].asCString(), "windows") == 0) {
							tmpI = 0;
						}
					}
					else tmpI = 0;
				}
			}
			if (tmpI == 0) continue;
		}
		strcpy(libDir, "libraries\\");
		tmpC = (char*)malloc(strlen(versionLib["path"].asCString()) + 2);
		strcpy(tmpC, versionLib["path"].asCString());
		strcat(libDir, tmpC);
		if (i.isMember("natives")) {
			//* Extract Native Libraries
		}
		for (int i = 0; i < strlen(libDir); i++) {
			if (libDir[i] == '/') libDir[i] = '\\';
		}
		available[theVersion] = libDir;
	}
	tmp.clear();
	for (Json::Value i : available) {
		tmp.append(i.asCString());
	}
	available.empty();
	char fvJar[512];
	strcpy(fvJar, fvJson);
	strcpy(fvJar + strlen(fvJson), ".jar\0");
	tmp.append(fvJar);
	tmpC = (char*)malloc(20480);
	join(tmp, tmpC, 20480, ";");
	writeLog("launchInstance", tmpC);
	SetWindowTextA(edit, tmpC);

	// Get Launch Level

	bool level;
	if (versionInfo.isMember("arguments")) level = 1;
	else if (versionInfo.isMember("minecraftArguments")) level = 0;
	else {
		writeLog("launchInstance", "Unknow launch level, default to 0. ");
		level = 0;
	}
	libraries.empty();
	tmp.empty();
	versionLib.empty();
	libNameSp.empty();

	Json::Value gameArg;
	char* gameArgC = nullptr;
	Json::Value jvmArg;
	char* jvmArgC = nullptr;
	if (level == 0) {
		gameArgC = (char*)malloc(strlen(versionInfo["minecraftArguments"].asCString())+2);
		strcpy(gameArgC, versionInfo["minecraftArguments"].asCString());
		jvmArg.clear();
		jvmArg.append("-cp");
		jvmArg.append(tmpC);
	}
	if (level == 1) {
		for (Json::Value i : versionInfo["arguments"]["game"]) {
			if (i.isString()) gameArg.append(i);
			else {
				tmp = i["value"];
				for (Json::Value j : tmp) {
					gameArg.append(j);
				}
			}
		}
		gameArgC = (char*)malloc(strlen(versionInfo["minecraftArguments"].asCString()) + 2);
		join(gameArg, gameArgC, strlen(versionInfo["minecraftArguments"].asCString()), " ");
		jvmArg.clear();
		for (Json::Value i : versionInfo["arguments"]["jvm"]) {
			if (i.isString()) jvmArg.append(i);
			else {
				tmp = i["value"];
				tmpI = 0;
				for (Json::Value j : i["rules"]) {
					if (!j.isMember("os")) continue;
					if (strcmp(j["action"].asCString(), "allow") == 0) {
						if (j.isMember("os")) {
							if (strcmp(j["os"]["name"].asCString(), "windows") == 0)
								tmpI = 1;
						}
						else {
							tmpI = 1;
						}
					}
					if (strcmp(j["action"].asCString(), "disallow")) {
						if (j.isMember("os")) {
							if (strcmp(j["os"]["name"].asCString(), "windows")) {
								tmpI = 0;
							}
						}
						else {
							tmpI = 0;
						}
					}
				}
			}
		}
	}
	jvmArgC = (char*)malloc(20480);
	join(jvmArg, jvmArgC, 20480, " ");
	strcpy(tmpS, "versions\\");
	strcat(tmpS, versionId);
	strcat(tmpS, "\\river_launch.bat");
	FILE* output = fopen(tmpS, "w");
	writeFile(output, "@echo off\ntitle Minecraft Log\ncd /d ");
	writeFile(output, cwd);
	writeFile(output, "\n");

	// Get Javas

	return 0;
}