
#include "portab.h"
#include "machine.h"
#include "dosbind.h"
#include "gsdefs.h"
#include "gsevars.h"


/// 8086 memory model bits //////////////////////////////////////////////

void far *ADDR(void *p)
{
	return (void far *)p;
}

void LBCOPY(void far *d, void far *s, unsigned count)
{
	unsigned char far *dest = (unsigned char far *)d;
	unsigned char far *src  = (unsigned char far *)s;
	/* TODO: Worth replacing with optimised asm? */
	while (count)
	{
		*dest = *src;
		++dest;
		++src;
		--count;
	}
}


void b_stuff(void far *ptr, int count, unsigned char value)
{
	/* TODO: Worth replacing with optimised asm? */
	int n;
	unsigned char far *p = (unsigned char far *)ptr;

	for (n = 0; n < count; n++) p[n] = value;	
}


/// PC hardware bits ////////////////////////////////////////////////////

#define VID_A_MDA 0
#define VID_A_CGA 1
#define VID_A_EGA 2
#define VID_A_VGA 3

unsigned char vid_type(void)
{
	union REGS inregs,outregs;
/*
    Video adaptor type determination:

    Set AX=1200, BL=32 and call INT 10.
*/
	inregs.x.ax=0x1200;
	inregs.h.bl=0x32;
	int86(0x10,&inregs,&outregs);

/*  If AL returns 12, you have a VGA.  If not, set AH=12, BL=10 and call 
    INT 10 again. */

    	if (outregs.h.al==0x12) return VID_A_VGA;
    	inregs.h.ah=0x12;
    	inregs.h.bl=0x10;
	int86(0x10,&inregs,&outregs);

/* If BL returns 0,1,2,3, you have an EGA with 64,128,192,256K memory.  
   If not, set AH=0F and call INT 10 a third time.  */

	if (outregs.h.bl < 4) return VID_A_EGA;

   	inregs.h.ah=0x0F;
   	int86(0x10,&inregs,&outregs);
/*   	
   If AL is 7, you have an MDA (original monochrome adapter) or Hercules; 
   if not, you have a CGA.
 */
 	if (outregs.h.al == 7) return VID_A_MDA;
 	return VID_A_CGA;
}

unsigned char ibm_type(void)
{
	unsigned char far *rom = MK_FP(0xffff, 0);
	return rom[0x0E];
}


void setmode(int m)
{
	union REGS rg;

	rg.x.ax = m & 0xFF;
	int86(0x10, &rg, &rg);
}

int getmode()
{
	union REGS rg;

	rg.h.ah = 0x0f;
	int86(0x10, &rg, &rg);
	return rg.h.al;
}



void beeper(void)
{
_asm {
	cli
	mov dx, 50h
	in  al, 0x61
	and al, 0xFE
beep1:
	or  al, 2
	out 0x61, al
	mov cx, 300
beep2:
	loop	beep2
	and al, 0xFD
	out 0x61, al
	mov cx, 1000
beep3:
	loop	beep3
	dec dx
	jnz beep1
	sti
}
}

void printn(const char *s)
{
	union REGS rgi, rgo;

	rgi.x.ax = 0x0E00;
	rgi.x.bx = 7;
	while (*s)
	{
		if (*s == '\n')
		{
			rgi.h.al = '\r';
			int86(0x10, &rgi, &rgo);
		}
		rgi.h.al = *s;
		int86(0x10, &rgi, &rgo);
		++s;
	}
}


void printa(int attr, const char *s)
{
	union REGS rgi, rgo;

	while (*s)
	{
		if (*s == '\n')
		{
			rgi.h.ah = 0x0E;
			rgi.x.bx = 7;
			rgi.h.al = '\r';
			int86(0x10, &rgi, &rgo);
			rgi.h.al = '\n';
			int86(0x10, &rgi, &rgo);
			continue;
		}
		rgi.h.ah = 9;
		rgi.h.bl = attr;
		rgi.h.bh = 0;
		rgi.x.cx = 1;
		int86(0x10, &rgi, &rgo);
		rgi.h.ah = 0x0E;
		rgi.h.al = *s;
		rgi.x.bx = 7;
		int86(0x10, &rgi, &rgo);
		++s;
	}
}

void cur_set(int top, int bottom)
{
	union REGS rg;

	rg.h.ch = top;
	rg.h.cl = bottom;
	rg.h.ah = 1;
	int86(0x10, &rg, &rg);	
}

void pos_cur(int row, int col)
{
	union REGS rg;

	rg.h.dh = row;
	rg.h.dl = col;
	rg.h.ah = 2;
	rg.h.bh = 0;
	int86(0x10, &rg, &rg);	
}

int read_chr(void)
{
	union REGS rg;
	
	rg.h.ah = 2;	/* Cursor to (0,0) */
	rg.h.bh = 0;
	rg.x.dx = 0;
	int86(0x10, &rg, &rg);	
	rg.h.ah = 8;
	rg.h.bh = 0;
	int86(0x10, &rg, &rg);	
	return rg.x.ax;
}

int get_row(void)
{
	union REGS rg;

	rg.h.ah = 3;
	rg.h.bh = 0;
	int86(0x10, &rg, &rg);
	return rg.h.dh;
}

void cls(int r1, int c1, int r2, int c2)
{
	union REGS rg;

	rg.h.ch = r1;
	rg.h.cl = c1;
	rg.h.dh = r2;
	rg.h.dl = c2;
	rg.x.ax = 0x600;
	rg.h.bh = gl_colours[0];

	int86(0x10, &rg, &rg);
}

int get_key(void)
{
	union REGS rg;

	rg.x.ax = 0x0C06;
	intdos(&rg, &rg);
	rg.x.ax = 0;
	int86(0x16, &rg, &rg);
	return rg.x.ax;
}


/* Can't load the EGA chargen using the int86() helper, because it doesn't 
 * allow BP to be set.
 *
 * Original DRI logo:
 * -#######|#######-  7F FE
 * #-------|-------#  80 01
 * #-------|-------#  80 01
 * #--####-|-####--#  9E 79
 * #--####-|-#--#--#  9E 49
 * #--####-|-#--#--#  9E 49
 * #--####-|-#--#--#  9E 49
 * #--####-|-#--#--#  9E 49
 * #--####-|-#--#--#  9E 49
 * #--####-|-####--#  9E 79
 * #-------|-------#  80 01
 * #-------|-------#  80 01
 * -#######|#######-  7F FE
 *
 * FreeGEM logo:
 * --------|--------  00 00
 * --------|--------  00 00
 * ---#####|#####---  1F F8
 * --##----|----##--  30 0C
 * -##-----|-----##-  60 06
 * -##-----|-----##-  60 06
 * --##----|----##--  30 0C
 * ---##---|---##---  18 18
 * ----##--|--##----  0C 30
 * -----##-|-##-----  06 60
 * ------##|##------  03 C0
 * -------#|#-------  01 80
 * --------|--------  00 00
 * --------|--------  00 00
 *
 * Other user-defined characters:
 *  Tick            Box
 * -------- 0x00   ######## 0xFF
 * -------# 0x01   ######## 0xFF
 * -------# 0x01   ##----## 0xC3
 * ------## 0x03   ##----## 0xC3
 * ------#- 0x02   ##----## 0xC3
 * -----##- 0x06   ##----## 0xC3
 * ##---#-- 0xC4   ##----## 0xC3
 * -#--##-- 0x4C   ##----## 0xC3
 * -##-#--- 0x68   ##----## 0xC3
 * --###--- 0x38   ##----## 0xC3
 * ---##--- 0x30   ##----## 0xC3
 * ---#---- 0x10   ##----## 0xC3
 * -------- 0x00   ######## 0xFF
 * -------- 0x00   ######## 0xFF
 */

#if FREEGEM_SETUP
// FreeGEM logo

char lbits[]={	0x00, 0x00, 0x1F, 0x30, 0x60, 0x60, 0x30,
		0x18, 0x0C, 0x06, 0x03, 0x01, 0x00, 0x00,
		0x00, 0x00, 0xF8, 0x0C, 0x06, 0x06, 0x0C,
		0x18, 0x30, 0x60, 0xC0, 0x80, 0x00, 0x00};

#else /* FREEGEM_SETUP */

// DRI logo

char lbits[]={	0x7F, 0x80, 0x80, 0x9E, 0x9E, 0x9E, 0x9E,
		0x9E, 0x9E, 0x9E, 0x9E, 0x80, 0x80, 0x7F,
		0xFE, 0x01, 0x01, 0x79, 0x49, 0x49, 0x49,
		0x49, 0x49, 0x49, 0x79, 0x01, 0x01, 0xFE};

#endif /* FREEGEM_SETUP */

char checkb[]={
		0x00, 0x01, 0x01, 0x03, 0x02, 0x06, 0xC4,
		0x4C, 0x68, 0x38, 0x30, 0x10, 0x00, 0x00,
		0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
		0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF};

void prn_logo(void)
{

_asm {
	push	bp
	mov	bp, offset lbits
	mov	ax, seg lbits
	mov	es, ax
	mov	cx, 2		;Count of chars to redefine
	mov	dx, 0xDF	;First ascii char to redefine
	mov	bl, 0
	mov	bh, 14		;Height of character bitmap
	mov	al, 0
	mov	ah, 0x11
	int	0x10
	mov	bp, offset checkb
	mov	ax, seg checkb
	mov	es, ax
	mov	cx, 2		;Count of chars to redefine
	mov	dx, 16		;First ascii char to redefine
	mov	bl, 0
	mov	bh, 14		;Height of character bitmap
	mov	al, 0
	mov	ah, 0x11
	int	0x10
	pop	bp
}
}



/// PCDOS bindings /////////////////////////////////////////////////////////

unsigned char gl_dta[128];

int dos_xfcb(unsigned char *xfcb, unsigned char *dta)
{
	union REGS rg;
	struct SREGS sg;
	rg.x.ax = 0x1100;
	rg.x.bx = FP_OFF(dta);
	rg.x.dx = FP_OFF(xfcb);
	sg.ds = FP_SEG(xfcb);
	intdosx(&rg, &rg, &sg);
	return rg.x.ax & 0xFF;
}

int dos_16h(unsigned char *xfcb)
{
	union REGS rg;
	struct SREGS sg;
	rg.x.ax = 0x1600;
	rg.x.dx = FP_OFF(xfcb);
	sg.ds = FP_SEG(xfcb);
	intdosx(&rg, &rg, &sg);
	return rg.x.ax & 0xFF;
}

int dos_ren(unsigned char *dta)
{
	union REGS rg;
	struct SREGS sg;
	rg.x.ax = 0x1700;
	rg.x.dx = FP_OFF(dta);
	sg.ds = FP_SEG(dta);
	intdosx(&rg, &rg, &sg);
	return rg.x.ax & 0xFF;
}

void dos_sdta(void *dta)
{
	union REGS rg;
	struct SREGS sg;
	rg.x.ax = 0x1A00;
	rg.x.dx = FP_OFF(dta);
	sg.ds = FP_SEG(dta);
	intdosx(&rg, &rg, &sg);
}

void dos_space(int drive, long *ptotal, long *pavail)
{
	union REGS rg;

	rg.x.ax = 0x3600;
	rg.x.dx = drive;
	intdos(&rg, &rg);

	*ptotal = (long)rg.x.ax * (long)rg.x.cx * (long)rg.x.dx;
	*pavail = (long)rg.x.ax * (long)rg.x.cx * (long)rg.x.bx;
}

int dos_mkdir(const char *filename)
{
	union REGS rg;
	struct SREGS sg;
	rg.x.ax = 0x3900;
	rg.x.dx = FP_OFF(filename);
	sg.ds   = FP_SEG(filename);
	intdosx(&rg, &rg, &sg);
	return (!rg.x.cflag);
}


int dos_create(const char *filename, int mode)
{
	union REGS rg;
	struct SREGS sg;
	rg.x.ax = 0x3C00;
        rg.x.cx = mode;
	rg.x.dx = FP_OFF(filename);
	sg.ds   = FP_SEG(filename);
	intdosx(&rg, &rg, &sg);
	if (rg.x.cflag) return -1;
	return rg.x.ax;
}


int dos_open(const char *filename, int mode)
{
	union REGS rg;
	struct SREGS sg;
	rg.x.ax = 0x3D00 | mode;
	rg.x.dx = FP_OFF(filename);
	sg.ds   = FP_SEG(filename);
	intdosx(&rg, &rg, &sg);
	if (rg.x.cflag) return -1;
	return rg.x.ax;
}

int dos_close(int handle)
{
	union REGS rg;

	rg.x.ax = 0x3e00;
	rg.x.bx = handle;
	intdos(&rg, &rg);
	return (!rg.x.cflag);
}


int dos_dele(const char *filename)
{
	union REGS rg;
	struct SREGS sg;

	rg.x.ax = 0x4100;
	rg.x.dx = FP_OFF(filename);
	sg.ds   = FP_SEG(filename);
	intdosx(&rg, &rg, &sg);
	return rg.x.ax;
}

int dos_sfirst(const char *filename, int attr)
{
	union REGS rg;
	struct SREGS sg;
	rg.x.ax = 0x4E00;
	rg.x.cx = attr;
	rg.x.dx = FP_OFF(filename);
	sg.ds   = FP_SEG(filename);
	intdosx(&rg, &rg, &sg);
	return (!rg.x.cflag);
}

int dos_snext()
{
	union REGS rg;

	rg.x.ax = 0x4F00;
	intdos(&rg, &rg);
	return (!rg.x.cflag);
}

long dos_fsize(const char *name)
{
	unsigned char dta[128];
	long size;

	dos_sdta(dta);
	if (!dos_sfirst(name, 0)) return 0;

	size = ((long)dta[26]) | 
		(((long)dta[27]) << 8) |
		(((long)dta[28]) << 16) |
		(((long)dta[29]) << 24);

	dos_sdta(gl_dta);
	return size;
}



int dos_sdrv(int drive)
{
	union REGS rg;
	rg.x.ax = 0x0E00;
	rg.x.dx = drive;
	intdos(&rg, &rg);
	return rg.h.al;
}


int dos_gdrv(void)
{
	union REGS rg;
	rg.x.ax = 0x1900;
	intdos(&rg, &rg);
	return rg.h.al;
}

void dos_gdir(int drive, char *buf)
{
	union REGS rg;
	struct SREGS sg;

	rg.x.ax = 0x4700;
	rg.x.dx = drive;
	rg.x.si = FP_OFF(buf);
	sg.ds = FP_SEG(buf);
	intdosx(&rg, &rg, &sg); 
}

int dos_parse(char *name, unsigned char *fcb, unsigned char options)
{
	union REGS rg;
	struct SREGS sg;

	rg.x.ax = 0x2900 | options;
	rg.x.si = FP_OFF(name);
	rg.x.di = FP_OFF(fcb);
	sg.ds   = FP_SEG(name);
	sg.es   = FP_SEG(fcb);
	intdosx(&rg, &rg, &sg);

	if (rg.x.cflag) return -1;
	if (rg.h.al == 0xFF) return -1;
	return rg.h.al;
}

static int DOS_ERR;

static unsigned read_piece(int handle, unsigned count, unsigned char far *pbuffer)
{
	union REGS rg;
	struct SREGS sg;

	rg.x.ax = 0x3F00;
	rg.x.bx = handle;
	rg.x.cx = count;
	rg.x.dx = FP_OFF(pbuffer);
	sg.ds   = FP_SEG(pbuffer);
	intdosx(&rg, &rg, &sg);
	DOS_ERR = rg.x.cflag;
	return rg.x.ax;
}



static unsigned write_piece(int handle, unsigned count, unsigned char far *pbuffer)
{
	union REGS rg;
	struct SREGS sg;

	rg.x.ax = 0x4000;
	rg.x.bx = handle;
	rg.x.cx = count;
	rg.x.dx = FP_OFF(pbuffer);
	sg.ds   = FP_SEG(pbuffer);
	intdosx(&rg, &rg, &sg);
	DOS_ERR = rg.x.cflag;
	return rg.x.ax;
}

long dos_read(int handle, long cnt, void far *p)
{
	unsigned char far *pbuffer = (unsigned char far *)p;
	unsigned buff_piece;
	unsigned long rd_count;

	buff_piece = 0x8000;
	rd_count = 0L;
	DOS_ERR = 0;
	while (cnt && !DOS_ERR)
	{
		if (cnt > 0x8000L) 
		{
			cnt -= 0x8000L;
		}
		else
		{
			buff_piece = cnt;
			cnt = 0;
		}
		rd_count += read_piece(handle, buff_piece, pbuffer);
		pbuffer += 0x8000;	
	}
	return rd_count;
}


long dos_write(int handle, long cnt, void far *p)
{
	unsigned char far *pbuffer = (unsigned char far *)p;
	unsigned buff_piece;
	unsigned long wt_count;

	buff_piece = 0x8000;
	wt_count = 0L;
	DOS_ERR = 0;
	while (cnt && !DOS_ERR)
	{
		if (cnt > 0x8000L) 
		{
			cnt -= 0x8000L;
		}
		else
		{
			buff_piece = cnt;
			cnt = 0;
		}
		wt_count += write_piece(handle, buff_piece, pbuffer);
		pbuffer += 0x8000;	
	}
	return wt_count;
}




int N_FDRV(void)
{
	union REGS rg;

	int86(0x11, &rg, &rg);
	if (rg.x.ax & 1)
	{
		return ((rg.x.ax & 0xC0) >> 6) + 1;
	}
	return 0;
}

int dos_version()
{
	union REGS rg;

	rg.x.ax = 0x3000;
	intdos(&rg, &rg);
	return (rg.h.al << 8) | (rg.h.ah >> 8);
}

/* Returns 1 if drive fixed, 0 if removable, -1 if no such drive */
int dos_probedrive(int drive)
{
	union REGS rg;

	rg.x.ax = 0x4408;
	rg.h.bl = drive;
	intdos(&rg, &rg);

	if (rg.x.cflag && rg.x.ax == 15) return -1;

	return rg.x.ax;
}


void dos_getdt(int handle, unsigned *time, unsigned *date)
{
	union REGS rg;

	rg.x.ax = 0x5700;
	rg.x.bx = handle;
	intdos(&rg, &rg);

	*time = rg.x.cx;
	*date = rg.x.dx;
}

void dos_setdt(int handle, unsigned time, unsigned date)
{
	union REGS rg;

	rg.x.ax = 0x5701;
	rg.x.bx = handle;
	rg.x.cx = time;
	rg.x.dx = date;
	intdos(&rg, &rg);
}

long dos_avail(void)
{
	union REGS rg;

	rg.x.ax = 0x4800;
	rg.x.bx = 0xFFFF;
	intdos(&rg, &rg);

	return ((long)rg.x.bx) << 4;
}

char far *dos_alloc(long count)
{
	union REGS rg;

	rg.x.ax = 0x4800;
	if (count == -1) rg.x.bx = 0xFFFF;
	else		 rg.x.bx = (count + 15) >> 4;	

	intdos(&rg, &rg);
	if (rg.x.cflag) return 0;
	return MK_FP(rg.x.ax, 0);
}

int dos_free(char far *addr)
{
	union REGS rg;
	struct SREGS sg;

	sg.es = FP_SEG(addr);
	rg.x.ax = 0x4900;
	intdosx(&rg, &rg, &sg);

	return rg.x.ax;
}

/* Detect the Windows NT DOS box. If found, we're almost certainly running
 * in a window and redefining characters won't work. */
int windows_nt(void)
{
	union REGS rg;

	rg.x.ax = 0x3306;
	intdos(&rg, &rg);

	return (rg.x.bx == 0x3205);
}


void detect_drives(unsigned long *pfloppies, unsigned long *phard)
{
	int drv_count = dos_sdrv(dos_gdrv());
	int flp_count = N_FDRV();
	unsigned long mask;
	int drive;
	int version = dos_version();

	// If there's only one floppy drive, start at C: so that we don't
	// try to probe B: //
	if (flp_count == 1)
	{
		*pfloppies = 0x80000000L;
		mask       = 0x20000000L;
		drv_count -= 2;
		drive      = 3;
	}
	// This assumes all floppies come before all hard drives. 
	// Not true since DOS 4.x, but the DOS 2.x probe below can't
	// distinguish hard from floppy drives, so keep it in for those
	// systems. //
	else if (version < 0x300)
	{
		int n;

		mask  = 0x80000000L;
		for (n = 0; n < flp_count; n++)
		{
			*pfloppies |= mask;
			mask = mask >> 1;
		}
		drv_count -= flp_count;
		drive  = flp_count + 1;
	}
	else
	{
		mask = 0x80000000L;
		drive = 1;
	}
	if (version >= 0x300)
	{
		while (mask && drv_count)
		{
			switch (dos_probedrive(drive))
			{
				case 0: *pfloppies |= mask; --drv_count; break;	
				case 1: *phard     |= mask; --drv_count; break;	
			}
			++drive;
			mask = mask >> 1;
		}
	}
	else	// DOS 2.x probe can't distinguish hard/floppy, so assume //
	{	// that floppies were detected higher up and these are all //
		// hard drives //
		char buf[8];
		unsigned char fcb[37];

		strcpy(buf, "A:\\*.*");
		while (mask && drv_count)
		{
			buf[0] = drive + '@';
			if (dos_parse(buf, fcb, 1) >= 0)
			{
				*phard     |= mask; 
				--drv_count;
			}
			mask = mask >> 1;
			++drive;
		}	
	}
}
/*

void get_source_path()
{
	char *p;

	source_path[0] = dos_gdrv() + 'A';
	source_path[1] = ':';
	source_path[2] = '/';
	dos_gdir(0, source_path + 3);
	p = source_path + strlen(source_path) - 1;
	if (*p != '/' && *p != '\\') strcat(source_path, "/");
	detect_drives(&floppy_drives, &hard_drives);
}

*/

