#pragma once
#include <river/defines.h>
#define JAVA17 "https://ghproxy.com/https://github.com/adoptium/temurin17-binaries/releases/download/jdk-17.0.7+7/OpenJDK17U-jdk_x64_windows_hotspot_17.0.7_7.msi"
#define JAVA8 "https://ghproxy.com/https://github.com/adoptium/temurin8-binaries/releases/download/jdk8u372-b07/OpenJDK8U-jre_x64_windows_hotspot_8u372b07.msi"

/* Deprecated 
int XX_downloadJava(char level) {
	mkdir("java");
	FILE* fp = fopen("javaTemp.zip", "wb");
	switch (level) {
	case 17: {
		Response resp = Get(JAVA17);
		fwrite(resp.GetBinary(), 1, resp.size(), fp);
		break;
	}
	case 8: {
		Response resp = Get(JAVA8);
		fwrite(resp.GetBinary(), 1, resp.size(), fp);
		break;
	}
	}
	fclose(fp);
	return 0;
}
*/