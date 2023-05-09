#define _CRT_SECURE_NO_WARNINGS

#define __CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#include "translator.h"

int main(int argc, char* argv[])
{
	FILE* inFile = fopen("data.txt", "r");
	FILE* outFile = fopen("result.txt", "w");

	if (inFile && outFile)
		translateCode(inFile, outFile);

	if (inFile) fclose(inFile);
	if (outFile) fclose(outFile);

	_CrtDumpMemoryLeaks();
	return 0;
}