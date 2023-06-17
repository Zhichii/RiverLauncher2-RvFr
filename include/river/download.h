#pragma once
#include <river/defines.h>

int download(HWND win, HWND btn) {
	thread thr([=]()->int {
		pgbDownload->set(0);
		char versionId[256];
		lisMinecraftDownloads->getText(lisMinecraftDownloads->getSelIndex(), versionId);
		char tmpS[512];
		GetWindowTextA(*ediSettingsDir, tmpS, 512);
		strcatf(tmpS, "\\versions\\%s", versionId);
		MessageBoxA(win, tmpS, "Test", MB_OK | MB_ICONINFORMATION);
		int index = lisMinecraftDownloads->getSelIndex();

		// Get Json

		Response resp = Get(versionManifest["versions"][index]["url"].asCString());
		char tmpS2[512];
		strcpyf(tmpS2, "%s\\%s.json", tmpS, versionId);
		MakeSureDirectoryPathExists(tmpS2);
		Json::Value tempValue;
		reader.parse(resp.GetText(), tempValue);
		Json::StyledWriter sw;
		FILE* fp = fopen(tmpS2, "w");
		Json::String s = sw.write(tempValue);
		fwrite(s.c_str(), 1, s.size(), fp);
		fclose(fp);
		pgbDownload->add(10);

		// Get Libraries
		char tmpS3[512];
		libraries = 0;
		GetWindowTextA(*ediSettingsDir, tmpS3, 512);
		double step = 30.0 / tempValue["libraries"].size();
		double progress = 10;

		for (int i = 0; i < tempValue["libraries"].size(); i++) {
			mtx.lock();
			libraries++;
			mtx.unlock();
			char* temp;
			try {
				if (tempValue["libraries"][i]["downloads"].isMember("artifact")) {
					strcpyf(tmpS2, "%s\\libraries\\%s", tmpS3, tempValue["libraries"][i]["downloads"]["artifact"]["path"].asCString());
					for (int j = 0; j < 512; j++) {
						if (tmpS2[j] == '/') tmpS2[j] = '\\';
					}
					int size;
					winGetHttps(&temp, tempValue["libraries"][i]["downloads"]["artifact"]["url"].asCString(), &size);
					MakeSureDirectoryPathExists(tmpS2);
					FILE* fp = fopen(tmpS2, "wb");
					fwrite(temp, 1, size, fp);
					fclose(fp);
					free(temp);
				}
				else if (tempValue["libraries"][i]["downloads"].isMember("classifiers") &&
					tempValue["libraries"][i]["downloads"]["classifiers"].isMember("natives-windows")) {
					strcpyf(tmpS2, "%s\\libraries\\%s", tmpS3, tempValue["libraries"][i]["downloads"]["classifiers"]["natives-windows"]["path"].asCString());
					for (int j = 0; j < 512; j++) {
						if (tmpS2[j] == '/') tmpS2[j] = '\\';
					}
					char* temp;
					int size;
					winGetHttps(&temp, tempValue["libraries"][i]["downloads"]["classifiers"]["natives-windows"]["url"].asCString(), &size);
					MakeSureDirectoryPathExists(tmpS2);
					FILE* fp = fopen(tmpS2, "wb");
					fwrite(temp, 1, size, fp);
					fclose(fp);
					free(temp);
				}
				else;
			}
			catch (const char* msg) {
				DebugBreak();
			};
			progress += step;
			pgbDownload->set((int)progress);
			UpdateWindow(*pgbDownload);
		}
		while (libraries < tempValue["libraries"].size()) {
			Sleep(5);
			writeLog("thr", "%d", libraries);
		}
		pgbDownload->set(40);

		// Get Game Jar
		Json::Value jar = tempValue["downloads"]["client"];
		resp = Get(jar["url"].asCString());
		strcpyf(tmpS2, "%s\\%s.jar", tmpS, versionId);
		fp = fopen(tmpS2, "wb");
		fwrite(resp.GetBinary(), 1, resp.size(), fp);
		fclose(fp);
		pgbDownload->add(25);

		// Get Logging
		if (tempValue.isMember("logging")) {
			Json::Value tmp = tempValue["logging"]["client"]["file"];
			resp = Get(tmp["url"].asCString());
			strcpyf(tmpS2, "%s\\%s", tmpS, tmp["id"].asCString());
			fp = fopen(tmpS2, "w");
			fwrite(resp.GetText().c_str(), 1, resp.size(), fp);
			fclose(fp);
		}
		pgbDownload->add(5);

		// Finish

		pgbDownload->set(100);
		strcpyf(tmpS, "Finished Download '%s'. ", versionId);
		MessageBoxA(win, tmpS, "Hint", MB_OK | MB_ICONINFORMATION);
		pgbDownload->set(0);

		return 0;
		});
	thr.detach();
	return 0;
}
