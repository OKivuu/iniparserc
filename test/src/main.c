/*
 * main.c
 *
 *	Created on: 18 de jun de 2019
 *			Author: OKivuu
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "iniparserc.h"

#define TEST_FILES_PATH "../samples/"
#define TEST_MAX_RANGE 70

static void create_log_folder();
static void print_map(char** map, const char* testLabel);

int main(int argc, char *argv[])
{
	char** map = NULL;
	char string [256] = {0};
	FILE_HANDLE testFile = INVALID_HANDLE;
	char testFileName [32] = {0};
	char test [16] = {0};
	int t = 10;

	create_log_folder();

	while (t < TEST_MAX_RANGE)
	{
		if (!map)
		{
			ini_mem_alloc((void**)&map, 1*sizeof(char*));

			if (!map)
				return 0;
		}

		sprintf (test, "t%d", t);
		sprintf (testFileName, "%s%s.ini", TEST_FILES_PATH, test);

		ini_fs_open(&testFile, testFileName);

		if (testFile)
		{
			printf ("Executing %s test...\n", test);

			map = IniParserC_GetSession((const char*) testFileName, "MYSESSION", map, 1);
			print_map(map, test);
			t++;

			ini_fs_close(&testFile);
		}
		else
		{
			t=(t-(t%10))+10;
		}

		if (map != NULL)
		{
			int i = 0;

			for (; map[i] != NULL; i++)
			{
				ini_mem_free(&map[i]);
				map[i] = NULL;
			}
		}
	}

	printf ("Message on Exit:\n");
	gets (string);
	printf ("Exiting with: %s",string);
	return 0;
}

void create_log_folder()
{
	struct stat st = {0};

	if (stat("./log", &st) == -1)
	{
		mkdir("./log");
	}
}

void print_map(char** map, const char* testLabel)
{
	FILE_HANDLE logFile = INVALID_HANDLE;
	char logFileName [32] = {0};

	sprintf(logFileName, "./log/%s_log.txt", testLabel);

	ini_fs_open(&logFile, logFileName);

	if (logFile)
	{
		ini_fs_close(&logFile);
		remove(logFileName);
	}

	logFile = fopen(logFileName, "w+");

	if (map != NULL && logFile != INVALID_HANDLE)
	{
		int i = 0;

		if (map[0] != NULL && map[1] != NULL)
		{
			while(map[i] != NULL && map[i+1] != NULL)
			{
				fprintf(logFile, "[%s] result in map { %s : %s }\n", testLabel, map[i], map[i+1]);
				i+=2;
			}
		}
		else
		{
			fprintf(logFile, "[%s] result in empty map\n", testLabel);
		}

		ini_fs_close(&logFile);
	}
}

