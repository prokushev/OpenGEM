/*	DOSBIND.H	4/18/84 - 9/07/84	Lee Lorenzen		*/

EXTERN unsigned char gl_dta[] ;

#define STDIN 0x0000
#define STDOUT 0x0001
#define STDERR 0x0002
#define STDAUX 0x0003
#define STDPRN 0x0004

#define E_BADFUNC 1
#define E_FILENOTFND 2
#define E_PATHNOTFND 3
#define E_NOHANDLES 4
#define E_NOACCESS 5
#define E_BADHANDLE 6
#define E_MEMBLKERR 7
#define E_NOMEMORY 8
#define E_BADMEMBLK 9
#define E_BADENVIR 10
#define E_BADFORMAT 11
#define E_BADACCESS 12
#define E_BADDATA 13
#define E_BADDRIVE 15
#define E_NODELDIR 16
#define E_NOTDEVICE 17
#define E_NOFILES 18

#define F_RDONLY 0x01
#define F_HIDDEN 0x02
#define F_SYSTEM 0x04
#define F_VOLUME 0x08
#define F_SUBDIR 0x10
#define F_ARCHIVE 0x20

#define F_GETMOD 0x0
#define F_SETMOD 0x1

typedef struct Xfcb_blk
{
    unsigned char fc_flag;
    BYTE fc_rsv[5];
    BYTE fc_vol_attr;
    BYTE fc_drv;
    BYTE fc_name[11];
    WORD fc_cblock;
    WORD fc_rsize;
    LONG fc_fsize;
    WORD fc_date;
    WORD fc_time;
    BYTE fc_resv[8];
    BYTE fc_crec;
    LONG fc_rnum;
} XFCB;

typedef struct data_trans
{
    BYTE dta_reserved[21];
    BYTE dta_attr;
    BYTE dta_time[2];
    BYTE dta_date[2];
    LONG dta_size;
    BYTE dta_name[13];
} DTA;

typedef struct fcb_blk
{
    BYTE fc_drv;
    BYTE fc_name[11];
    WORD fc_cblock;
    WORD fc_rsize;
    LONG fc_fsize;
    WORD fc_date;
    WORD fc_time;
    BYTE fc_resv[8];
    BYTE fc_crec;
    LONG fc_rnum;
} FCB;

typedef struct exec_blk
{
	WORD		eb_segenv;
	LONG		eb_pcmdln;
	LONG		eb_pfcb1;
	LONG		eb_pfcb2;
} EXEC_BLK;


typedef struct over_blk
{
	WORD		ob_seglod;
	WORD		ob_relfac;
} OVER_BLK;

EXTERN  WORD dos_cin(void);
EXTERN  WORD dos_getim(int handle);
EXTERN 	WORD dos_label();	/* dosbind.lib */
EXTERN 	void dos_space(int drive, long *ptotal, long *pavail);
EXTERN 	WORD dos_rmdir();
EXTERN 	WORD dos_chdir();
EXTERN 	void dos_gdir(int drive, char *buf);
EXTERN 	void dos_sdta(void *dta);
EXTERN 	WORD dos_sfirst(const char *filename, int attr);
EXTERN 	WORD dos_snext();
EXTERN 	int  dos_gdrv(void);
EXTERN 	int  dos_sdrv(int drive);
EXTERN 	WORD dos_create(const char *filename, int mode);
EXTERN 	WORD dos_mkdir(const char *filename);
EXTERN 	WORD dos_open(const char *filename, int mode);
EXTERN 	WORD dos_delete();
EXTERN 	WORD dos_rename();
EXTERN 	WORD dos_close(WORD handle);
EXTERN 	LONG dos_read(int handle, long cnt, void far *buffer);
EXTERN 	LONG dos_write(int handle, long cnt, void far *buffer);
EXTERN 	FBYTE *dos_lseek();
EXTERN 	VOID dos_exec();
EXTERN 	WORD dos_wait();
EXTERN 	LONG dos_envir();
EXTERN 	WORD dos_chmod();
EXTERN 	FBYTE *dos_alloc(long bytes);
EXTERN 	WORD dos_free(FBYTE *mem);
EXTERN 	void dos_getdt(int handle, unsigned *time, unsigned *date);
EXTERN 	void dos_setdt(int handle, unsigned time, unsigned date);
EXTERN 	WORD dos_find();
EXTERN  LONG dos_gint();
EXTERN  VOID dos_sint();
EXTERN	VOID dos_fdup();
EXTERN  LONG dos_avail(void) ;
EXTERN  LONG dos_fsize(const char *filename) ;
EXTERN  WORD dos_version() ;
EXTERN  int  dos_probedrive(int drive);
EXTERN  WORD dos_xfcb(unsigned char *fcb, unsigned char *dta) ;
EXTERN  WORD dos_16h(unsigned char *fcb) ;
EXTERN  WORD dos_dele(const char *filename) ;
EXTERN  WORD dos_ren(unsigned char *dta) ;
EXTERN  int  dos_parse(char *name, unsigned char *fcb, unsigned char options) ;
