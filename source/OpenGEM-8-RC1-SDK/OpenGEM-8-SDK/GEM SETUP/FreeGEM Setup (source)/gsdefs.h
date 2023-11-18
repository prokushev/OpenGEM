/************************************************************************/
/**       Copyright 1999, Caldera Thin Clients, Inc.                     **/ 
/**       This software is licenced under the GNU Public License.        **/ 
/**       Please see LICENSE.TXT for further information.                **/ 
/**                                                                      **/ 
/**                  Historical Copyright                                **/ 
/**									**/
/*	Copyright (c) 1987 Digital Research Inc.		        */
/*    The software contained in this listing is proprietary to       	*/
/*    Digital Research Inc., Pacific Grove, California and is        	*/
/*    covered by U.S. and other copyright protection.  Unauthorized  	*/
/*    copying, adaptation, distribution, use or display is prohibited	*/
/*    and may be subject to civil and criminal penalties.  Disclosure	*/
/*    to others is prohibited.  For the terms and conditions of soft-	*/
/*    ware code use refer to the appropriate Digital Research        	*/
/*    license agreement.						*/
/************************************************************************/

/* Let's try pulling in some bits of the Pacific C standard library */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

#define FREEGEM_SETUP 1	 /* 0: Install Digital Research GEM/3
                          * 1: Install FreeGEM */

/* GSDEFS.h	typedefs and defines for GEMSETUP.EXE.			*/

#define		COPY_BSIZE	0x10000L
#define		MIN_MEM		0xA000L

#define		NUM_REL_DSKS	8
#define		FOOTERS_SZ	3
#define		PROMPTS_SZ	69
#define		CHOICES_SZ	28
#define		MX_CHOICES	20
#define		MX_SCREENS	45
#define		MX_MICE		10
#define		MX_CATEGORIES	15 	/* how many '@' in gemsetup.txt? */
					/* also add '@' in any driver pks */
#define		HELP_SCREEN	36

#define		INDENT		5		/* Column of choice print */
#define		REG_COL		gl_colours[0]	/* Regular text attribute */
#define		CHOS_COL	gl_colours[1]	/* Red	  		  */
#define		CHK_COL		gl_colours[2]	/* Reverse video red	  */
#define		REV_COL		gl_colours[3]	/* Reverse video regular  */
#define		LOGO_COL	gl_colours[4]	/* Colour for logo        */
#define         HEADING_COL     gl_colours[5]   /* Colour for heading     */
#define         BULLET_COL      gl_colours[6]   /* Colour for checkbox    */

#define		NO_ERR_EXPECTED	1 
#define		END_OF_SCRNS	-1
#define		NO_ESC_DEFINED  1

#define		PIPE		'|'
#define		BEG_BRACE       '{'
#define		END_BRACE       '}'
#define		SLASH		'\\'
#define		UCASE_A		'A'
#define 	UCASE_C 	'C'
#define		LCASE_Z		'z'
#define		ASTERISK	'*'
#define		COLON		':'
#define		SEMICOLON	';'
#define		DOT	 	'.'
#define		COMMA		','
#define		W_DOT		"."
#define 	UCASE_D 	'D'
#define 	UCASE_W 	'W'
#define 	UCASE_M 	'M'
#define		AT_SIGN		'@'
#define		SHARP_SIGN	'#'
#define		SPACE		0x20
#define		TAB		0x09
#define		CR		0x0d
#define		LF		0x0a
#define		CNTL_Z		0x1a
#define		ESCAPE		0x011b
#define		F1		0x3B00
#define		UP_ARROW	0x4800
#define		DOWN_ARROW	0x5000

#define		SCRN_TYPE	1
#define		PRNT_TYPE	21
#define		PLOT_TYPE	11
#define		CMRA_TYPE	41
#define		SCAN_TYPE	51
#define		MOUS_TYPE	301
#define		META_TYPE	31
#define		NO_MOUSE	0 
#define		BUS_MOUSE	2
#define		PS2_MOUSE	10
#define		NO_PORT		0xFF

#define		DUAL_FLOPPY	1
#define		SING_FLOPPY	21
#define		HARD_DISK	41

#define		HIDE_FALSE	-6
#define		HIDE_TRUE	-5

#define		MX_DEVICES	max_devs
#define		FNAME_LENGTH	67
#define		NAME_LENGTH	13
#define		DESC_LENGTH	80
#define		PROMPT_LEN	80
#define		CHOICE_LEN	80
#define		MX_HEX		10	/* ~max length of a hex string */
#define		HALF_LINE	40	

#define		READ		0 	/* file access code for dos_open() */
#define		READWRITE	0	/* file ATTRIBUTE code dos_create() */

typedef struct _dev_struct
{
    WORD	type ;
    BOOLEAN	installed ;
    BOOLEAN	fnts_installed ;
    BOOLEAN	change ;
    WORD	ioport ;
    WORD	mouse ;
    BYTE	f_name[ NAME_LENGTH ] ;
    BYTE	s_desc[ NAME_LENGTH ] ;
    BYTE	desc[ DESC_LENGTH + 1 ] ;
    BYTE	m_desc[ DESC_LENGTH + 1 ] ;
    BYTE	src_disk[ HALF_LINE ] ;
    BYTE	font_disk[ HALF_LINE ] ;
    BYTE	font_wcard[ HALF_LINE ] ;
    BYTE	scan_port[ FNAME_LENGTH ] ;
    BYTE	scan_xfer[ FNAME_LENGTH ] ;
    BYTE	scan_com[ NAME_LENGTH ] ;
    BYTE	aux_file[ FNAME_LENGTH ] ;
    BYTE	src_path[ FNAME_LENGTH ];
} DEV_STRUCT ;

typedef struct _action
{
    VOID	(*todo_func)(VOID) ;
    WORD	next ;
    WORD	err_next ;
} ACTION_REC ;

typedef struct _screen_record
{
    VOID	(*setup_func)(VOID) ;
    WORD	num_choices ;
    WORD	strt_choice ;
    BYTE	*prompt ;
    BYTE	*choices ;
    BYTE	*footer ;
    WORD	esc_next ;
    ACTION_REC	choice[ MX_CHOICES ] ;
} SCREEN_REC ;    

BOOLEAN get_vol ( BYTE *label_str, BYTE *src_path);
BOOLEAN get_vol2( BYTE *vol_str, BYTE *src_path);
WORD is_vol( BYTE *vol_str, BYTE *src_path);
int open_file(const char *path, const char *name, int mode);
int create_file(const char *path, const char *name, int mode);
BOOLEAN file_copy(const char *src, const char *dst, const char *name, BOOLEAN patch);
BOOLEAN append_text_buf(const char *src);
VOID cpy_dsktop(const char *src);
VOID cpy_strtup(const char *src);
VOID copy_scan_com(void);
VOID get_gstxt_vals(WORD picked);
VOID next_dlabel(BYTE **dlabel, BYTE **dsrcpath);

/* IBM hardware access */
void cur_set(int top, int bottom);
void pos_cur(int row, int col);
void printa(int attr, const char *s);
int read_chr(void);
void b_stuff(void far *ptr, int count, unsigned char value);
unsigned char ibm_type(void);
int N_FDRV(void);
void cls(int r1, int c1, int r2, int c2);
unsigned char vid_type(void);
int getmode(void);
void setmode(int mode);
int windows_nt(void);
void detect_drives(unsigned long *pfloppies, unsigned long *phard);

/* Video types */
#define VID_A_MDA 0
#define VID_A_CGA 1
#define VID_A_EGA 2
#define VID_A_VGA 3

/* Colours: 
#define COLOUR_NORMAL	 0x17	// White on black. TODO: a background  //
				// other than black may not work well. //
#define COLOUR_INVERSE   0x71	// Reverse video //
#define COLOUR_CHECKED	 0xF2	// Cursor: Blinking red on white //
#define COLOUR_SELECTED  0x1B	// Selected menu line 
#define COLOUR_LOGO	 0x1B	// Colour for GEM logo: Bright red on black //
#define COLOUR_HEADING	 0x1A	// Colour for heading: Cyan //
#define COLOUR_BULLET	 0x12	// Colour for checkbox
*/

/* end of gsdefs.h */
