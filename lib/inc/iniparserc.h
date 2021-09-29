/*
 * iniparserc.h
 *
 *  Created on: 2 de nov de 2017
 *      Author: EKivuu
 */

#ifndef INC_INIPARSERC_H_
#define INC_INIPARSERC_H_

#include <stdio.h>
#include <stdlib.h>

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef NULL
#define NULL ( (void *) 0)
#endif
#define FILE_HANDLE                   FILE*
#define INVALID_HANDLE                NULL

#define ini_fs_open(h, filename)      (*(h) = fopen(filename, "r+"))
#define ini_fs_seek(h, pos, orig)     (fseek(*(h), pos, orig))
#define ini_fs_tell(h)                (ftell(*(h)))
#define ini_fs_read(h, buf, sz)       (fread(buf, 1, sz, *(h)))
#define ini_fs_close(h)               (fclose(*(h)))

#define ini_mem_alloc(p, s)           (*(p) = calloc(1, s))
#define ini_mem_realloc(p, s)         (*(p) = realloc(*(p), s))
#define ini_mem_free(p)     	        (free((void*)*(p)))

char** IniParserC_GetSession(const char* iniFile, const char* sectionDlls, char** map, int trimRight);

#endif /* INC_INIPARSERC_H_ */
