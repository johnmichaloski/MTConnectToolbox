#ifndef INIFILE_H
#define INIFILE_H

/*
  inifile.h

  Decls for INI file format functions
*/

#include <stdio.h> 		/* FILE */

#define INIFILE_MAX_LINELEN 1024 /* max number of chars in a line */

#define COMMENT_CHAR ';'	/* signifies a comment */

#define INI_OK      0
#define INI_DEFAULT 1
#define INI_INVALID 2

typedef struct {
  char tag[INIFILE_MAX_LINELEN];
  char rest[INIFILE_MAX_LINELEN];
} INIFILE_ENTRY;

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

extern const char * ini_find(FILE * fp,	/* already opened file ptr */
			     const char * tag,	/* string to find */
			     const char * section);	/* section it's in */

extern int ini_section(FILE * fp,	/* already opened file ptr */
		       const char * section,	/* section you want */
		       INIFILE_ENTRY array[],	/* entries to fill */
		       int max);	/* how many you can hold */

extern int ini_match(const char * match, const char * me);

extern int ini_has_section(FILE *fp, const char *section);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif
