#include "StdAfx.h"

#include "tes4qlod_small.h"
#include "llguicommands.h"

int cleanup_list_x[1048576];
int cleanup_list_y[1048576];
int cleanup_list_count = 0;

struct cell_data cell;

int input_count;
char const *input_filename[256];

int wrld_count;
char *wrld_name[10000];
char *wrld_description[10000];
char *wrld_formid[10000];
char *wrld_mod[10000];
int wrld_child[10000];



char *opt_tes_mode = TES_UNKNOWN;
int tes_rec_offset = TES4_OB_RECORD_SIZE; 

/***********************
 ** Function Prototypes.
 **********************/


/*********************************************************************
 ** 1. main(): The Main routine processes command line options,
 **            reads tes4qlod_GAME_ltex.dat file (contains base 
 **            LTEX (land texture FormIDs and filenames from the 
 **            original ESMs - this saves having to include the ESM
 **            each time you run this program.
 **
 **            Then calls ExportTES4LandT4QLOD() to parse the ESM/ESPs.
 **            Then calls HumptyLODs() to assemble all the temporary
 **            cell texture and normal BMP files in to quads and (if 
 **            requested) a full map. 
 **            Optionally calls HumptyVWD() to assemble the VWD records.
 *********************************************************************/

int tes4qlod_small()
{
	wrld_count =0;

	int in_group = 0,   /* Are we in a GRUP record group or not? */
	    size,           /* Size of current record.             */
	    group_size = 0,
	    pos = 0;

	char c;		/* For command-line getopts args.  */

	/***********************************
	 * Parse the command line arguments.
	 **********************************/
	
	if (gamemode == 2) {
		opt_tes_mode = TES_OBLIVION;
		tes_rec_offset = TES4_OB_RECORD_SIZE;
	}
	if (gamemode == 3) {
		opt_tes_mode = TES_FALLOUT3;
		tes_rec_offset = TES4_FA_SK_RECORD_SIZE;
	}
	if (gamemode == 4) {
		opt_tes_mode = TES_FALLOUTNV;
		tes_rec_offset = TES4_FA_SK_RECORD_SIZE;
	}
	if (gamemode == 5) {
		opt_tes_mode = TES_SKYRIM;
		tes_rec_offset = TES4_FA_SK_RECORD_SIZE;
	}

	/*
	 * Initialize variables:
	 */

//	printf("X offset will be %d. Y will be %d\n", opt_x_offset, opt_y_offset);

	cell.name[0] = '\0';
	cell.current_x = 0;
	cell.current_y = 0;

	return 0;
}


/************************************************************************************************
 ** ExportTES4LANDT4QLOD(): Export the texture details and/or VWD objects from ESM/ESPs.
 **
 **  Opens the specified TES file as input, parses looking for CELL, LAND, LTEX and FRMR records, 
 **  reads each record in to memory for processingm handing them on to one of:
 **                Process4CELLData(), Process4LANDData(), 
 **                Process4LTEXData(), Process4FRMRData().
 **
 **  1. Process4CELLData() retrieves the X and Y co-ordinate of the cell.
 **  2. Process4LTEXData() retrieves the land textute FormID and filename.
 **  3. Process4LANDData() saves the normal and colour map data, and matches texture layer FormIDs
 **     with the bitmap images, producing a BMP for each cell.
 **  4. Process4FRMRData() retrieves placed object data; FormID and whether they're VWD.
 ***********************************************************************************************/

const char *current_esp;

int ExportTES4LandT4QLOD(const char *input_esp_filename, llLogger * mesg)
{
	current_esp = input_esp_filename;

	int size = 0,	   /* Size of current record.               */
	    pos = 0;
	char s[40];	/* For storing the record header.     */
	char s1[5] = "none";
	char *r;	/* Pointer to the Record Data.        */

	FILE *fpin; /* Input File Stream (original ESP/ESM).  */

	if ((fpin = fopen(input_esp_filename, "rb")) == NULL) {
		mesg->WriteNextLine(LOG_ERROR, "Unable to open %s for reading: %s\n",
			input_esp_filename, strerror(errno));
		return 1;
	}

	while (fread(s, sizeof( char ), 8, fpin) > 0) {

		if (!isalpha(s[0]) && !isalpha(s[1]) && !isalpha(s[2]) && !isalpha(s[3])) {
			mesg->WriteNextLine(LOG_ERROR, " - WARNING: FOUND A WILD NON-ASCII RECORD HEADER in file %s, last rec was %c%c%c%c\n", input_esp_filename, s1[0], s1[1], s1[2], s1[3]);
			fclose(fpin);
			return 1;
		}

		s1[0] = s[0];
		s1[1] = s[1];
		s1[2] = s[2];
		s1[3] = s[3];


		/**************************************
		 * The Core TES4 ESM/ESP Record Parser.
		 *************************************/

		if (strncmp(s, "TES4", 4) == 0 ||
			strncmp(s, "GRUP", 4) == 0) {
			size = tes_rec_offset;
		} else if ( 
			strncmp(s, "HEDR", 4) == 0 ||
			strncmp(s, "OFST", 4) == 0 ||
			strncmp(s, "MAST", 4) == 0 ||
			strncmp(s, "DATA", 4) == 0 ||
			strncmp(s, "DELE", 4) == 0 ||
			strncmp(s, "CNAM", 4) == 0 ||
			strncmp(s, "INTV", 4) == 0 ||
			strncmp(s, "INCC", 4) == 0 ||
			strncmp(s, "ONAM", 4) == 0 ||
			strncmp(s, "SNAM", 4) == 0) {

			size = 0;
			memcpy(&size, (s+4), 2);
			size += 6;

		} else  {
			memcpy(&size, (s+4), 4);
			size += tes_rec_offset;
		}

		/************************************
		 * End of Core TES4 Parser.
		 ***********************************/

		/************************************
		 * Keep seeking to the next record if
		 * it's of no interest to us.
		 ***********************************/

		if (strncmp(s, "CELL", 4) != 0 &&
			strncmp(s, "LAND", 4) != 0 &&
			strncmp(s, "WRLD", 4) != 0 &&
			strncmp(s, "TXST", 4) != 0 &&
			strncmp(s, "LTEX", 4) != 0) {
			if (fseek(fpin, size-8, SEEK_CUR)) {
				fclose(fpin);
				mesg->WriteNextLine(LOG_ERROR,"Seek failed\n");
				return 1;
			}
			continue;
		}

		/**********************************************
		 * It must be one we need to process, so create
		 * some memory space to store the record.
		 *********************************************/

		if ((r = (char *) malloc(size)) == NULL) {
			fclose(fpin);
			mesg->WriteNextLine(LOG_ERROR, "Unable to allocate %d bytes of memory to store TES file record: %s\n",
				2*size, strerror(errno));
			return 1;
		}

		if (fseek(fpin, -8, SEEK_CUR)) {
			fclose(fpin);
			mesg->WriteNextLine(LOG_ERROR, "Seek failed\n");
			return 1;
		}

		if (fread(r, 1, size, fpin) < size) {
			fclose(fpin);
			mesg->WriteNextLine(LOG_ERROR, 
				"Unable to read entire marker record (%d bytes) from %s into memory: %s\n", size, input_esp_filename, strerror(errno));
			return 1;
		}
		pos+= 6 + size;

		/******************************************************
		 ** If it's a CELL or a LAND record, then hand it on to
		 ** a procedure that will handle the format, including
		 ** determining if any modifications should be made.
		 *****************************************************/

		if (strncmp(s, "WRLD", 4) == 0) {
			if (!Process4WRLDData(r, size)) {
				//mesg->WriteNextLine(MH_ERROR,"Process WRLD");
				//total_worlds++;
			}
		} 

		free(r);
	}
	fclose(fpin);

	return 0;
}


/*****************************************************************
** 5. Process4WRLDData(): Process a TES4 Worldspace record.
*****************************************************************
** WRLD (4 bytes) + Length (2 bytes) + EDID (name)
****************************************************************/

int Process4WRLDData(char *r, int size)
{
	int  i;
	int  nsize, nsize2, nsize3;
	int  pos = 0,
	     xypos = 0;

	char lod2_dir[256];

	pos += tes_rec_offset;


	/***********************************************
	 * EDID (name of the Worldspace (6+Name bytes)).
	 **********************************************/
	if (strncmp("EDID", r + pos, 4) == 0) {

		wrld_name[wrld_count] = "none";
		wrld_formid[wrld_count] = new char[8];
		wrld_description[wrld_count] = "\0";
		wrld_child[wrld_count]=0;
		wrld_mod[wrld_count] = new char[strlen(current_esp)+1];
		strcpy(wrld_mod[wrld_count], current_esp);

		nsize = 0;
		memcpy(&nsize, r+pos+4, 2);
		pos += 6;

		wrld_name[wrld_count] = new char[nsize+1];
		strncpy(wrld_name[wrld_count], r + pos, nsize);
		wrld_name[wrld_count][nsize] = '\0';

		pos += nsize;		

		sprintf(wrld_formid[wrld_count],"%2.2X%2.2X%2.2X%2.2X", 
			(unsigned char) (r[15]),
			(unsigned char) (r[14]),
			(unsigned char) (r[13]),
			(unsigned char) (r[12]));

		

		if (strncmp("FULL", r + pos , 4) == 0) {
			nsize2 = 0;
			memcpy(&nsize2, r+pos+4, 2);
			pos += 6;

			wrld_description[wrld_count] = new char[nsize2+1];
			strncpy(wrld_description[wrld_count], r + pos, nsize2);
			wrld_description[wrld_count][nsize2] = '\0';
			pos += nsize2;
		} 
	

		if (strncmp("WNAM", r + pos , 4) == 0) {
			wrld_child[wrld_count]=1;
			nsize3 = 0;
			memcpy(&nsize3, r+pos+4, 2);
			pos += 6;
			pos += nsize3;
		} 


		//check if wrld is already there...
		int p=0;
		for (int j=0;j<wrld_count;j++) {
			if (strcmp(wrld_formid[wrld_count],wrld_formid[j]) == 0 //&& 
				//strcmp(wrld_mod[wrld_count],wrld_mod[j]) == 0
				) {
				if (strlen(wrld_description[wrld_count])) wrld_description[j] = wrld_description[wrld_count];
				p=1;
			}
		}
		if (!p) { //new....
			wrld_count++;
		}
		
	}

	return 0;
}




int StringToFormID(char *formid, char *s)
{
        int j;
        char htmp[3];
        char c;

        for (j = 0; j < 4; j++) {
                htmp[0] = s[(2*j)];
                htmp[1] = s[(2*j)+1];
                htmp[2] = '\0';

                if (islower(htmp[0])) htmp[0] = toupper(htmp[0]);
                if (islower(htmp[1])) htmp[1] = toupper(htmp[1]);

                sscanf(htmp, "%X", &c);
                formid[j] = c;
	}

	return 0;
}

int StringToReverseFormID(char *formid, char *s)
{
        int j;
        char htmp[3];
        char c;

        for (j = 0; j < 4; j++) {
                htmp[0] = s[(2*j)];
                htmp[1] = s[(2*j)+1];
                htmp[2] = '\0';

                if (islower(htmp[0])) htmp[0] = toupper(htmp[0]);
                if (islower(htmp[1])) htmp[1] = toupper(htmp[1]);

                sscanf(htmp, "%X", &c);
                formid[3-j] = c;
        }

        return 0;
}

int StringToHex(char *shex, char *s, int size)
{
        int j;
        char htmp[3];
        char c;

        for (j = 0; j < 3; j++) {
                htmp[0] = s[(2*j)];
                htmp[1] = s[(2*j)+1];
                htmp[2] = '\0';

                if (islower(htmp[0])) htmp[0] = toupper(htmp[0]);
                if (islower(htmp[1])) htmp[1] = toupper(htmp[1]);

                sscanf(htmp, "%X", &c);
                shex[2-j] = c;
	}
	return 0;
}

