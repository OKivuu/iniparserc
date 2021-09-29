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
#define TEST_ASSERT_FILES_PATH "../assert/"
#define TEST_MAX_RANGE 80

static void create_log_folder();
static void print_map(char** map, const char* testLabel);
static void assert_log_files(const char* testLabel);

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
			printf ("Executing %s test...", test);

			map = IniParserC_GetSession((const char*) testFileName, "MYSESSION", map, 1);
			print_map(map, test);
			assert_log_files(test);
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

void assert_log_files(const char* testLabel)
{
	char ch1 = '\0';
	char ch2 = '\0';
	int flag = 0;
	int line = 0;
	int col = 0;
	int bytes = 0;
	FILE_HANDLE fp1 = NULL;
	FILE_HANDLE fp2 = NULL;
	char fp1Name [32] = {0};
	char fp2Name [32] = {0};

	sprintf(fp1Name, "./log/%s_log.txt", testLabel);
	sprintf(fp2Name, "%s%s_log.txt", TEST_ASSERT_FILES_PATH, testLabel);

	ini_fs_open(&fp1, fp1Name);
	ini_fs_open(&fp2, fp2Name);

	do
	{
		ch1 = fgetc(fp1);
		ch2 = fgetc(fp2);

		// Increment line
		if (ch1 == '\n' || ch1 == '\r')
		{
			line += 1;
			col = 0;
		}

		if(ch1 != ch2)
		{
			ini_fs_seek(&fp1, -1, SEEK_CUR);
			ini_fs_seek(&fp2, -1, SEEK_CUR);
			flag = 1;
			break;
		}

		col++;
		bytes++;

	} while (ch1 != EOF && ch2 != EOF);

	if (flag)
	{
		printf("\nAssert failed! Line: %d, col: %d (bytes:%d)\n", line, col, bytes);
	}
	else
	{
		printf(" OK!\n", ftell(fp1)+1);
	}

	ini_fs_close(&fp1);
	ini_fs_close(&fp2);
}

