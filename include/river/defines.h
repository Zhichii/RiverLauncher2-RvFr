#include <stdio.h>
#include <json/json_reader.cpp>
#include <json/json_writer.cpp>
#include <json/json_value.cpp>
#include <json/json.h>
#include <urlmon.h>
#include <ctime>
#include <cstring>
#include <string>
#include <malloc.h>
#include <direct.h>
#include <fstream>
#include <string.h>
#include <windows.h>
#include <thread>
#include <sys/stat.h>
#include <cstdarg>
#define INT_MAX 2147483647
#define ALLOC 4096
#define DEBUG_MODE 0
using namespace std;

char name[15] = "RiverLauncher2";
Json::Reader reader;
Json::Value accounts;
FILE* programmeLog = fopen(".river_log.txt", "w");
int curPage = 0;
/*
 * 0: Minecraft [Download]
 * 1: Accounts [Login to accounts]
 * 2: Settings
 * 3: Connect [Create new rooms]
 */

int split(const char* baseStr, char* newStr, int newSize, int from = 0, int to = 0, int step = 1) {
	if (from == to) to = strlen(baseStr) - 1;
	if (from < 0) from = strlen(baseStr) + from;
	if (to < 0) to = strlen(baseStr) + to;
	if (step == 0) step = 1;
	memset(newStr, 0, newSize);

	int cur = 0;
	for (int i = from; (from < to) ? (i < to) : (i > to); i += step) {
		newStr[cur++] = baseStr[i];
	}
	newStr[cur] = 0;

	return 0;
}

// %% means "%". %d means "long long". %s means "char*". %c means "char". 
int writeLog(const char* logger, const char* format, ...) {
	char str[256];
	strcpy(str, "[%d, %s] ");
	va_list args;
	va_start(args, format);
	long long i;
	printf(str, time(NULL), logger);
	fprintf(programmeLog, str, time(NULL), logger);
	fflush(programmeLog);
	int fori;
	for (fori = 0; fori < strlen(format); fori++) {
		if (format[fori] == '%') {
			switch (format[fori + 1]) {
			case '%': {
				printf("%%", i);
				fprintf(programmeLog, "%%");
				fflush(programmeLog);
				break;
			}
			case 's': {
				i = va_arg(args, long long);
				printf("%s", i);
				fprintf(programmeLog, "%s", i);
				fflush(programmeLog);
				break;
			}
			case 'd': {
				i = va_arg(args, int);
				printf("%d", i);
				fprintf(programmeLog, "%d", i);
				fflush(programmeLog);
				break;
			}
			case 'l': {
				i = va_arg(args, long long);
				printf("%lld", i);
				fprintf(programmeLog, "%lld", i);
				fflush(programmeLog);
				break;
			}
			case 'c': {
				i = va_arg(args, char);
				printf("%c", i);
				fprintf(programmeLog, "%c", i);
				fflush(programmeLog);
				break;
			}
			default: {
				break;
			}
			}
			fori++;
		}
		else {
			printf("%c", format[fori]);
			fprintf(programmeLog, "%c", format[fori]);
			fflush(programmeLog);
		}
	}
	va_end(args);
	fprintf(programmeLog, "\n");
	fflush(programmeLog);
	return 0;
}

int find(const char* str, const char* substr) {
	for (int i = 0; i < strlen(str); i++) {
		for (int j = 0; j < strlen(substr); j++) {
			if ((i + j) > strlen(str)) break;
			if (DEBUG_MODE) writeLog("find", "%d, %d, %c, %c", i, j, str[i + j], substr[j]);
			if (str[i + j] != substr[j]) break;
			if (j == strlen(substr) - 1) {
				return i;
			}
		}
	}
	return -1;
}

int count(const char* str, const char* substr) {
	int i;
	int cnt = 0;
	for (i = 0; i < strlen(str); i++) {
		for (int j = 0; j < strlen(substr); j++) {
			if ((i + j) > strlen(str)) break;
			if (str[i + j] != substr[j]) break;
			if (j == strlen(substr) - 1) {
				cnt++;
				i += j;
				break;
			}
		}
	}
	return cnt;
}

int replace(const int newSize, const char* baseStr, char* newStr, const char* from, const char* to) {
	if (strlen(baseStr) < strlen(from)) {
		strcpy(newStr, baseStr);
		return 0;
	}
	const int cnt = count(baseStr, from);
	int i = 0, j = 0;
	const int baseLen = strlen(baseStr);
	const int fromLen = strlen(from);
	const int toLen = strlen(to);
	const int newLen = strlen(baseStr) + (toLen)*cnt + 1;

	char* tmp = (char*)malloc(newLen + 2);
	char tmp2[2];
	memset(tmp, 0, newLen);
	memset(newStr, 0, newSize);
	int cur = 0, curLast = 0;
	bool flag = 0;
	for (i = 0; i < strlen(baseStr); i++) {
		flag = 0;
		for (j = 0; j < strlen(from); j++) {
			if ((i + j) > strlen(baseStr)) break;
			if (DEBUG_MODE) writeLog("replace", "%d, %d, %c, %c, %s\t%s", i, j, baseStr[i + j], from[j], tmp, newStr);
			tmp[cur++] = baseStr[i + j];
			if (baseStr[i + j] != from[j]) {
				tmp2[0] = baseStr[i];
				tmp2[1] = 0;
				strcat(newStr, tmp2);
				memset(tmp, 0, newLen);
				cur = 0;
				break;
			}
			if (j == strlen(from) - 1) {
				flag = 1;
				strcat(newStr, to);
				memset(tmp, 0, newLen);
				i += j;
				break;
			}
		}
	}
	if (DEBUG_MODE) writeLog("replace", "%s\t%s", tmp, newStr);

	free(tmp);
	return 0;
}

int valueSplit(const char* str, const char* sep, Json::Value* output) {
	*output = Json::arrayValue;
	output->clear();
	if (strlen(str) < strlen(sep)) {
		output->append(str);
		return 0;
	}
	const int cnt = count(str, sep);
	int i = 0, j = 0;
	const int strLen = strlen(str);
	const int sepLen = strlen(sep);

	char* tmp = (char*)malloc(strLen + 2);
	char tmp2[2];
	char* tmp3 = (char*)malloc(strLen + 2);
	memset(tmp, 0, strLen);
	memset(tmp3, 0, strLen);

	int cur = 0, curLast = 0;
	bool flag = 0;
	for (i = 0; i < strLen; i++) {
		flag = 0;
		for (j = 0; j < sepLen; j++) {
			if ((i + j) > strLen) break;
			if (DEBUG_MODE) writeLog("valueSplit", "%d, %d, %c, %c, %s\t%s", i, j, str[i + j], sep[j], tmp, tmp3);
			tmp[cur++] = str[i + j];
			if (str[i + j] != sep[j]) {
				tmp2[0] = str[i];
				tmp2[1] = 0;
				strcat(tmp3, tmp2);
				memset(tmp, 0, strLen);
				cur = 0;
				break;
			}
			if (j == strlen(sep) - 1) {
				flag = 1;
				output->append(tmp3);
				memset(tmp, 0, strLen);
				memset(tmp3, 0, strLen);
				i += j;
				break;
			}
		}
	}
	output->append(tmp3);
	if (DEBUG_MODE) writeLog("valueSplit", "%s\t%s", tmp, tmp3);

	free(tmp);
	free(tmp3);
	return 0;
}

int join(Json::Value arr, char* output, int outputSize, const char* joiner) {
	memset(output, 0, outputSize);
	int flag = 0;
	for (Json::Value i : arr) {
		if (strlen(i.asCString()) == 0) continue;
		if (flag) {
			flag = 1;
			strcat(output, joiner);
		}
		strcat(output, i.asCString());
	}
	return 0;
}

int writeFile(FILE* stream, const char* str) {
	fwrite(str, 1, strlen(str), stream);
	return 0;
}

int sp(char* str, int strSize, const char* a, const char* b, char* output) {
	memset(output, 0, strSize);
	int aPos = find(str, a);
	if (DEBUG_MODE) writeLog("sp", "%d", aPos);
	if (aPos == -1) return 1;
	char* tmp = (char*)malloc(strSize + 2);
	split(str, tmp, strSize, aPos + strlen(a), strlen(str));
	int bPos = find(tmp, b);
	if (DEBUG_MODE) writeLog("sp", "%d", bPos);
	if (bPos == -1) return 1;
	split(tmp, output, strSize, 0, bPos);
	free(tmp);
	return 0;
}

int findFile(const char* fileEnding) {
	return 0;
}