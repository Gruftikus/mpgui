
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
//#include <zlib.h>
//#include <unistd.h>
#include "../../lltool/include/lllogger.h"
#include "../../lltool/include/llutils.h"

int ExportTES4LandT4QLOD(const char *input_esp_filename, llLogger * mesg);
int Process4WRLDData(char *r, int size);
int tes4qlod_small(void);

extern int input_count;
extern char const *input_filename[1000];

extern int wrld_count;
extern char *wrld_name[10000];
extern char *wrld_description[10000];
extern char *wrld_formid[10000];
extern char *wrld_mod[10000];
extern int wrld_child[10000];

extern char *opt_tes_mode;
extern int tes_rec_offset; 



/***************************************************************
 * Different games and record header sizes. Oblivion uses 20, 
 * all later games to date use 24.
 **************************************************************/

#define TES_SKYRIM    "Skyrim"
#define TES_MORROWIND "Morrowind"
#define TES_FALLOUT3  "Fallout3"
#define TES_FALLOUTNV "FalloutNV"
#define TES_OBLIVION  "Oblivion"
#define TES_UNKNOWN   "Unknown"

#define TES4_OB_RECORD_SIZE  20
#define TES4_FA_SK_RECORD_SIZE 24




