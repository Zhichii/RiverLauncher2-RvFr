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
using namespace std;

void* globalAllocList[ALLOC] = { 0 }; 
int allocCur = 0;
void** allocListCur = globalAllocList;
void** allocListCurOld = NULL;
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

int split(const char* baseStr, char* newStr, int from = 0, int to = 0, int step = 1) {
	if (from == to) to = strlen(baseStr) - 1;
	if (from < 0) from = strlen(baseStr) + from;
	if (to < 0) to = strlen(baseStr) + to;
	if (step == 0) step = 1;
	strset(newStr, 0);

	int cur = 0;
	for (int i = from; (from<to)?(i<to):(i>to); i = step) {
		newStr[cur++] = baseStr[i];
	}
	newStr[cur] = 0;

	return 0;
}

int writeLog(const char* logger, const char* format, ...) {
	char str[256];
	strcpy(str, "[%d, %s] ");
	va_list args;
	va_start(args, format);
	long long i;
	printf(str, time(NULL), logger);
	fprintf(programmeLog, str, time(NULL), logger);
	fflush(programmeLog);
	int i;
	for (i = 0; i < strlen(format); i++) {
		if (format[i] == '%') {
			switch (format[i + 1]) {
			case '%': {
				i = va_arg(args, long long);
				printf("%s", i);
				fprintf(programmeLog, "%s", i);
				fflush(programmeLog);
			}
			case 's': {
				i = va_arg(args, long long);
				printf("%s", i);
				fprintf(programmeLog, "%s", i);
				fflush(programmeLog);
				break;
			}
			case 'l': {
				i = va_arg(args, int);
				printf("%lld", i);
				fprintf(programmeLog, "%lld", i);
				fflush(programmeLog);
				break;
			}
			default: {
				break;
			}
			}
			i += 1;
		}
		else {
			printf("%c", format[i]);
			fprintf(programmeLog, "%c", format[i]);
			fflush(programmeLog);
		}
	}
	va_end(va);
	return 0;
}

int find(const char* str, const char* substr) {
	for (int i = 0; i < strlen(str)-strlen(substr)+1; i++) {
		for (int j = 0; j < strlen(substr); j++) {
			//fprintf(programmeLog, "%d, %d, %c, %c\n", i, j, str[i+j], substr[j]);
			//fflush(programmeLog);
			if (str[i + j] != substr[j]) break;
			if (j == strlen(substr)-1) {
				return i;
			}
		}
	}
	return -1;
}

int count(const char* str, const char* substr) {
	int i;
	int cnt = 0;
	for (i = 0; i < strlen(str) - strlen(substr) + 1; i++) {
		for (int j = 0; j < strlen(substr); j++) {
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
	const int newLen = strlen(baseStr) + (toLen) * cnt + 1;

	char* tmp = (char*)malloc(newLen+2);
	char tmp2[2];
	memset(tmp, 0, newLen);
	memset(newStr, 0, newSize);
	int cur = 0, curLast = 0;
	bool flag = 0;
	for (i = 0; i < strlen(baseStr) - strlen(from) + 1; i++) {
		for (j = 0; j < strlen(from); j++) {
			//fprintf(programmeLog, "%d, %d, %c, %c, %s\t%s\n", i, j, baseStr[i + j], from[j], tmp, newStr);
			//fflush(programmeLog);
			tmp[cur++] = baseStr[i+j];
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
	if (flag == 0) {
		cur = 0;
		for (i = strlen(baseStr) - strlen(from) + 1; i < strlen(baseStr); i++) {
			tmp[cur++] = baseStr[i];
		}
		strcat(newStr, tmp);
	}
	fprintf(programmeLog, "%s\t%s\n", tmp, newStr);
	fflush(programmeLog);

	free(tmp);
	return 0;
}