/*
 * iniparserc.c
 *
 *  Created on: 2 de nov de 2017
 *      Author: EKivuu
 */
#include <stdio.h>
#include <string.h>

/*
 * iniparserc.c
 *
 *  Created on: 2 de nov de 2017
 *      Author: EKivuu
 */
#include "../inc/iniparserc.h"

#define PARSER_STS_READ_SESSION 0x01
#define PARSER_STS_READ_KEY     0x02
#define PARSER_STS_READ_VALUE   0x04
#define PARSER_STS_IN           0x08
#define PARSER_STS_NOT_EMPTY    0x10

static int ini_fs_avail(FILE_HANDLE* h);
static char** IniMapAdd(char** map, const char* value);

char** IniParserC_GetSession(const char* iniFile, const char* sectionDlls, char** map, int trimRight)
{
  if (!map)
  {
    ini_mem_alloc((void**)&map, 1*sizeof(char*));
  }

  if (map)
  {
    FILE_HANDLE file;

    ini_fs_open(&file, iniFile);

    if (file)
    {
      char buffer[1025] = {0};
      char buffer_aux[1025] = {0};
      char *p;
      char *s;
      char *k;
      char *v;
      int i, lr, r, j, re;
      int status = 0x00;

      ini_fs_seek(&file, 0, SEEK_SET);
      while((r = ini_fs_avail(&file)) > 0 || strlen(buffer_aux))
      {
        lr = r;

        if ((j = strlen(buffer_aux)))
        {
          memset(buffer, 0, 1024);
          strncpy(buffer, buffer_aux, j);
        }

        if ((r = MIN(r, 1024-j)))
        {
          re = ini_fs_read(&file, buffer+j, r);
        }

        p = &buffer[0];

        if (r == 0)
        {
          status -= PARSER_STS_NOT_EMPTY;

          if (strlen(buffer_aux) > 0)
          {
            status &= ~PARSER_STS_IN; //turn off
          }
        }
        else if (lr == re) //if there is still same data in the end, but its the end
        {
          r = -1;
        }

        if (status & PARSER_STS_READ_SESSION)
          goto IN_SESSION;

        while(p)
        switch (*p)
        {
          case '#':
          {
            //exclude comments: starting lines with '#'
            while(p++ && *p && *p != '\r' && *p != '\n');

            if (!*p) goto PARSER_END;

          }break;
          case '[':
          {
            //search section
            s = ++p;
            for(i=0; *p && *p != ']'; p++, i++);

            if (!*p) goto PARSER_END;

            memset(buffer_aux, 0, 1025);
            strncpy(buffer_aux, s, i);

            if (strcmp(buffer_aux, sectionDlls) == 0)
            {
              p++; //skip ']'
              while(1)
              {
                status = 0x1F;

                IN_SESSION:{;}

                //skip CR, LF
                while(*p && (*p == '\r' || *p == '\n' || *p == ' ')) p++;
                if (!*p) goto PARSER_END;

                while (*p == '#')
                {
                  while(p++ && *p && *p != '\r' && *p != '\n'); //Jump till end of line
                  if (!*p) goto PARSER_END;
                  while(*p && (*p == '\r' || *p == '\n' || *p == ' ')) p++; //Jump to next line
                  if (!*p) goto PARSER_END;
                }

                //search key
                k = p;
                for(i=0; *p && *p != '=' && *p != '['; p++, i++);
                if (!*p) goto PARSER_END;
                if (*p=='[') goto PARSER_END; //found another session

                if (trimRight) for(; i>0 && *(k+i-1) == ' '; i--);

                if (status & PARSER_STS_READ_KEY)
                {
                  memset(buffer_aux, 0, 1025);
                  strncpy(buffer_aux, k, i);
                  map = IniMapAdd(map, buffer_aux);
                  status -= PARSER_STS_READ_KEY;
                  memset(buffer_aux, 0, 1025);
                }

                //search value
                v = ++p;
                for(i=0; *p && *p != '\r' && *p != '\n'; p++, i++);
                if (status & PARSER_STS_IN && !*p && r >= 0) goto PARSER_END;

                if (trimRight) for(; i>0 && *(v+i-1) == ' '; i--);

                if (status & PARSER_STS_READ_VALUE)
                {
                  memset(buffer_aux, 0, 1025);
                  strncpy(buffer_aux, v, i);
                  map = IniMapAdd(map, buffer_aux);
                  status -= PARSER_STS_READ_VALUE;
                  memset(buffer_aux, 0, 1025);
                }

                status -= PARSER_STS_READ_SESSION;

                if (!(status & PARSER_STS_IN)  || !*p) goto PARSER_END;
              }
            }
            else { memset(buffer_aux, 0, 1025); p++; if (!*p) goto PARSER_END; }

          }break;

          default:
            p++;
            if (!*p) goto PARSER_END;
          break;
        }


        PARSER_END:
        {
          if (r < 0) break;
          if (*p=='[') break;

          if (status & PARSER_STS_READ_KEY || status & PARSER_STS_READ_VALUE)
          {
            if (status & PARSER_STS_NOT_EMPTY)
            {
              memset(buffer_aux, 0, 1025);
            }

            //save last line
            p--;
            if (*p)
              for(i=0; *p != '\r' && *p != '\n'; p--, i++);
            else
              break;

            memset(buffer_aux, 0, 1025);
            strncpy(buffer_aux, ++p, i);
          }
        };

      }

      ini_fs_close(&file);
    }
  }

  return map;
}

static int ini_fs_avail(FILE_HANDLE* h)
{
  long p1, p2;
  ini_fs_seek(h, 0, SEEK_CUR);
  p1 = ini_fs_tell(h);
  ini_fs_seek(h, 0, SEEK_END);
  p2 = ini_fs_tell(h);
  ini_fs_seek(h, p1, SEEK_SET);
  return p2 - p1;
}

static char** IniMapAdd(char** map, const char* value)
{
  if (map != NULL)
  {
    int i = 0;
    int m = 0;

    while(map[i++] != NULL);

    m = ini_mem_realloc((void**)&map, (i+1)*sizeof(char*));
    m = ini_mem_alloc((void**)&map[i-1], strlen(value)+1);

    strcpy(map[i-1], value);
    map[i] = NULL;
  }

  return map;
}
