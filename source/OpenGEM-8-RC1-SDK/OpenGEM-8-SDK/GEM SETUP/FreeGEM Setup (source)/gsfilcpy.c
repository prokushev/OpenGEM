/************************************************************************/
/*       Copyright 1999, Caldera Thin Clients, Inc.                     */ 
/*       This software is licenced under the GNU Public License.        */ 
/*       Please see LICENSE.TXT for further information.                */ 
/*                                                                      */ 
/*                  Historical Copyright                                */ 
/*									*/
/*	COPYRIGHT (c) 1987 Digital Research Inc.		        */
/*    The software contained in this listing is proprietary to       	*/
/*    Digital Research Inc., Pacific Grove, California and is        	*/
/*    covered by U.S. and other copyright protection.  Unauthorized  	*/
/*    copying, adaptation, distribution, use or display is prohibited	*/
/*    and may be subject to civil and criminal penalties.  Disclosure	*/
/*    to others is prohibited.  For the terms and conditions of soft-	*/
/*    ware code use refer to the appropriate Digital Research        	*/
/*    license agreement.						*/
/************************************************************************/

/* gsfilcpy.c 		routines for copying files from release disks.	*/

#include "portab.h"
#include "machine.h"
#include "dosbind.h"
#include "gsdefs.h"
#include "gstxtdef.h"
#include "gsevars.h"

EXTERN  BOOLEAN	 lstr_cmp() ;			/* gsutil.c */
EXTERN  WORD	 _min(WORD a, WORD b); 		/* gsutils.c */
EXTERN  VOID	 b_stuff() ;			/* scnutil.asm */
EXTERN 	VOID 	 cls() ;			/* scrnutil.asm */
EXTERN  VOID     printn(const char *s);		/* scrnutil.asm */
EXTERN  VOID     printa();			/* scrnutil.asm */
EXTERN  VOID     beeper();			/* scrnutil.asm */
EXTERN  VOID     pos_cur() ;			/* scrnutil.asm */
EXTERN  WORD    get_row() ;			/* scrnutil.asm */
EXTERN  WORD    get_key() ;			/* scrnutil.asm */
EXTERN  VOID	add_device() ;			/* gsnewdev.c */
EXTERN  BYTE	*pos_dot() ;			/* gsdvinst.c */
EXTERN  VOID	del_file() ;			/* gsdvinst.c */



/*----------------------------------------------------------------------*/

/* Passed the printed name of a disk. Look up its label */
BOOLEAN get_vol( BYTE *label_str, BYTE *src_path)
{
	BYTE	*vol_str = NULLPTR ;
	WORD	ii;
	BOOLEAN found ;
	
	found = FALSE ;
	
	for ( ii = 0; ii < NUM_REL_DSKS + num_drvr_paks; ii++ )
	    if (!strcmp( disk_label[ ii ], label_str ) )
	        {
		vol_str = vol_label[ ii ] ;
		found = TRUE ;
		ii = NUM_REL_DSKS + num_drvr_paks ;
		}
	
	if (!found)
		vol_str = vol_label[NUM_REL_DSKS + num_drvr_paks] ;
	
	if (!get_vol2( vol_str, src_path))
		return(FALSE);
	
	return(TRUE);
	
		
} /* get_vol() */
/*----------------------------------------------------------------------*/
BOOLEAN get_vol2( BYTE *vol_str, BYTE *src_path)
{
	WORD vol ;

	vol = is_vol( vol_str, src_path );
	
	if (vol == 0 || vol == -1)
		return(FALSE);
	else
		return(TRUE);
} /* get_vol2 */

/*----------------------------------------------------------------------*/
WORD is_vol( BYTE *vol_str, BYTE *src_path)
{
	BYTE	temp[12] ;
	XFCB	xfcb;
	WORD	ret, ii;
	BYTE    drive_id = src_path[0];
	BYTE 	*p;


/* OK. This is where we try to bypass the annoying disk-label system; 
 * accept a file called GEMSETUP.LBL containing all the disk-labels 
 * this directory covers. */
	ii = open_file(src_path, gemsetlbl, READ);
	if (ii >= 0)
	{
		ret = dos_read(ii, sizeof(misc_choices), misc_choices);
		dos_close(ii);
		p = &misc_choices[0][0];
		p[ret] = 0;
/* Now search for volume labels. As in gemsetup.txt, they are denoted by 
 * a '{' symbol */
		while (p = strchr(p, '{'))
		{
			++p;
			if (!strncmp(p, vol_str, 11)) return TRUE;
		}
	}

 

	drive_id -= UCASE_A;
	drive_id++;
	
	dos_sdta( gl_dta ) ;
	
	xfcb.fc_flag = 0xff;
	b_stuff(xfcb.fc_rsv, 5, NULL);
	xfcb.fc_vol_attr = 8;
	xfcb.fc_drv = drive_id;
	b_stuff(xfcb.fc_name, 11, '?');

	
	ret = dos_xfcb(&xfcb.fc_flag, gl_dta);
	
	if (ret == 0x00ff)
		return(-1);
	else
	{
		for (ii = 0; ii < 11; ii++)
			temp[ii] = gl_dta[ii + 8];
		temp[11] = NULL;
		
		if (!strcmp(temp, vol_str))
			return(TRUE);
		else 
			return(FALSE);
	}
} /* is_vol() */

/*----------------------------------------------------------------------*/
    BOOLEAN
set_vol(vol_str, drive_id)
BYTE	*vol_str;
BYTE	drive_id;
{
	XFCB	xfcb;
	WORD	ret, ii;
	
	dos_sdta( gl_dta ) ;
	
	xfcb.fc_flag = 0xff;
	b_stuff(xfcb.fc_rsv, 5, NULL);
	xfcb.fc_vol_attr = 8;
	xfcb.fc_drv = drive_id - UCASE_A + 1;
	b_stuff(xfcb.fc_name, 11, '?');

	
	ret = dos_xfcb(&xfcb.fc_flag, gl_dta);
	
	if (ret == 0)
	{
		for (ii = 0 ; ii < 11; ii++)
			gl_dta[24 + ii] = *(vol_str + ii);
		ret = dos_ren(gl_dta);
		if (ret != 0)
			return(FALSE);
	}
	else if (ret == 0x00ff)
	{
		for (ii = 0 ; ii < 11; ii++)
			xfcb.fc_name[ii] = *(vol_str + ii);
		ret = dos_16h(&xfcb.fc_flag);
		if (ret != 0)
			return(FALSE);
	}
	return(TRUE);
}/*set_vol();*/

/*----------------------------------------------------------------------*/
    VOID
init_copy_echo() 
{
    cls( 3, 0, 24, 79 ) ;
    pos_cur( 4, 0 ) ;
    last_footer = NULLPTR ;
} /* init_copy_echo() */

/*----------------------------------------------------------------------*/
    VOID
do_echo(const BYTE *src, const BYTE *dst, const BYTE*f_name ) 
{
BYTE	*ptr, *end ;
WORD	num_found ;

    num_found = get_row();
    if (num_found > 23)
	    init_copy_echo();
    
    num_found = 0 ;
    b_stuff( misc_choices, sizeof( misc_choices ), SPACE ) ;
    strcpy( misc_choices[0], copy_echo ) ;
    end = &misc_choices[0][strlen( misc_choices[0] )] ;
    for (ptr = misc_choices[0]; ptr < end; ptr++)
        {
	if ( *ptr == PIPE ) 
	    {
	    if ( num_found == 0 )
		LBCOPY( ADDR( ptr ), ADDR( f_name ), strlen( f_name ) ) ;
	    else if ( num_found == 1 )
		LBCOPY( ADDR( ptr ), ADDR( src ), strlen( src ) ) ;
	    else if ( num_found == 2 )
		LBCOPY( ADDR( ptr ), ADDR( dst ), strlen( dst ) ) ;
  	    ++num_found ;
	    }
    }
    printn( misc_choices[0] ) ;
} /* do_echo() */


/*----------------------------------------------------------------------*/
    VOID
fin_copy_echo( ok, src, dst, f_name)
BOOLEAN		ok ;
BYTE	*src, *dst, *f_name ;
{
    WORD	in ;	
    if ( !ok )
	{
	in = get_row() ;
	if (in > 22 )
		{
		init_copy_echo();
		do_echo( src, dst, f_name ) ;
		}
        printa( REV_COL, failed ) ;
	beeper();
        printn( CRLF ) ;
	printn( (char *)go_on ) ;
	in = get_key() ;
	printn( CRLF ) ;
	}
    else
        printn( done ) ;
    printn( CRLF ) ;
} /* fin_copy_echo() */

/*----------------------------------------------------------------------*/
static unsigned drive_x = 0, drive_y = 0;

UWORD do_dt_patch( unsigned long drives, BYTE *icon_label, BYTE *flags )
{
	unsigned long mask;
	BYTE	drive_id ;
	FBYTE	*start, *end ;

    for ( start = cpy_bptr; *start != NULL; start++ ) 
        ;
    end = start ;
    mask = 0x80000000L ;
    drive_id = UCASE_A ;
    if (flags[1] == 'T') drive_id = SPACE;
    while ( mask != 0x0000 )
        {
	if ( mask & drives )
	    {
	    LBCOPY( end, ADDR( flags ), strlen( flags ) ) ;
/* Format of entry is "#MxxyyiiFF d label@ @" */
	    end[3] = drive_x + '0';
            end[5] = drive_y + '0';
	    ++drive_y;
	    if (drive_y == 4) { drive_y = 0; --drive_x; }
	    end += strlen( flags ) ;
	    *end++ = SPACE ;
	    *end++ = drive_id ;
	    *end++ = SPACE ;
	    LBCOPY( end, ADDR( icon_label ), strlen( icon_label ) ) ;
	    end += strlen( icon_label ) ;
	    *end++ = AT_SIGN ;
	    *end++ = SPACE ;
	    *end++ = AT_SIGN ;
	    *end++ = CR ;
	    *end++ = LF ;
	    *end = NULL ;
	    }
	++drive_id ;
	mask >>= 1 ;
	}
    return( (UWORD)( end - start ) ) ;
} /* do_dt_patch() */

/*----------------------------------------------------------------------*/
int open_file( const char *path, const char *name, int mode )
{
    char file_name[ FNAME_LENGTH ] ;

    strcpy( file_name, path ) ;
    strcat( file_name, name ) ;
    return( dos_open( file_name, mode ) ) ;
} /* open_file() */

/*----------------------------------------------------------------------*/
    BOOLEAN
dtinf_merge( src, size ) 
BYTE	*src ;
LONG	*size ;
{
WORD	src_handle, cnt ;
BOOLEAN found ;
BYTE	fname_o[FNAME_LENGTH], fname_n[FNAME_LENGTH];
FBYTE	*gptr1, *gptr2, *tmp_ptr, *line_ptr;
LONG	dt2_len, n_bytes;


	n_bytes = COPY_BSIZE - *size;
	src_handle = open_file(src, desk2topinf, READ);
	if (src_handle >= 0)
	{
		dt2_len = dos_read(src_handle, n_bytes, cpy_bptr + *size);
	}
        else return( TRUE );

        for (gptr1 = cpy_bptr; ((gptr1 - cpy_bptr) < (*size-1)) || (*gptr1 != 0x1A); gptr1++);
	*size = (gptr1 - cpy_bptr) + 1;
	
	gptr1 = cpy_bptr;
     
	found = FALSE ;
	while (!found)
		if ( *gptr1 != '#')
			gptr1++;
		else if ( *( gptr1 + 1 ) == 'G')
			found = TRUE ;
		     else 
			     gptr1++;
	
	for (gptr2 = cpy_bptr + *size; gptr2 < cpy_bptr + *size + dt2_len;)
		{
		while (*gptr2 != SPACE)
			gptr2++;
		gptr2++ ;
		cnt = 1;
		while ( *(gptr2 + cnt ) != AT_SIGN)
			cnt++;
		LBCOPY( ADDR( fname_n ), gptr2, cnt );
		fname_n[cnt] = NULL ;
		
		for (tmp_ptr = gptr1; tmp_ptr < cpy_bptr + *size;)
			{
			line_ptr = tmp_ptr ;

			while (*tmp_ptr != SPACE)
				tmp_ptr++;
			tmp_ptr++;
			cnt = 1;
			while ( *(tmp_ptr + cnt ) != AT_SIGN)
				cnt++;
			LBCOPY( ADDR( fname_n ), tmp_ptr, cnt );
			fname_o[cnt] = NULL ;
			if (!strcmp( fname_o, fname_n))
				{
				tmp_ptr = line_ptr + 1 ;
				cnt = 1;
				while(*tmp_ptr != '#')
					{
					tmp_ptr++;
					cnt++;
					}
     
            LBCOPY( line_ptr, tmp_ptr, cpy_bptr + *size + dt2_len - line_ptr);
				*size -= cnt;
				tmp_ptr = line_ptr ;
				}
			else
				while (*tmp_ptr != '#')
					tmp_ptr++;
		}
		while (*gptr2 != CR && *gptr2 != LF)
			gptr2++;
		gptr2 += 3;
		
		
			
	}
	*size += dt2_len ;
	return( TRUE );
} /* dtinf_merge */

/*----------------------------------------------------------------------*/
BOOLEAN dtinf_patch( LONG *size ) 
{
BOOLEAN		found ;
FBYTE		*ptr, *ptr2, *tmp ;
UWORD		byte_cnt, tmp_size ;

    ptr = cpy_bptr ;
    found = FALSE ;
    for ( found=FALSE, ptr = cpy_bptr; !found && ptr < cpy_bptr+ *size; ptr++ )
    	if ( *ptr == SHARP_SIGN )
	    found = *(ptr + 1) == UCASE_W ;
    for ( ; *ptr != COLON && ptr < cpy_bptr + *size; ptr++ )
        ;
    if (config == HARD_DISK)
	    *(ptr - 1) = DEFAULT_DRV[ 0 ] ;
    else 
	    *(ptr - 1) = UCASE_A ;
    for ( found = FALSE; !found && ptr < cpy_bptr + *size; ptr++ )
    	if ( *ptr == SHARP_SIGN )
	    found = *(ptr + 1) == UCASE_M ;
    ptr2 = ptr - 1 ;
    *ptr2 = NULL ;
    for ( found = FALSE; !found && ptr < cpy_bptr + *size; ptr++ )
    	if ( *ptr == SHARP_SIGN )
	    found = *(ptr + 1) != UCASE_M ;
    --ptr ;	    
    tmp = cpy_bptr + ( COPY_BSIZE - *size - 1 ) ;
    tmp_size = (UWORD)( *size - (ptr - cpy_bptr) ) ;
    LBCOPY( tmp, ptr, tmp_size ) ;
    drive_x = 7;
    drive_y = 0;
    byte_cnt = 0 ;		/* how many bytes are we adding to file? */
    if ( gl_flpydrives != 0x0000 )
	byte_cnt += do_dt_patch( gl_flpydrives, floppydisk, dtinf_fdisk ) ;
    if ( gl_harddrives != 0x0000 ) 
	byte_cnt += do_dt_patch( gl_harddrives, harddisk, dtinf_hdisk ) ;  
#if FREEGEM_SETUP
    drive_y = 3;
    byte_cnt += do_dt_patch(0x80000000L, trash, dtinf_trash);
#endif
    ptr2 += byte_cnt ;
    LBCOPY( ptr2, tmp, tmp_size ) ;
    *size += byte_cnt ;
    return( TRUE ) ;
} /* dtinf_patch() */

/*----------------------------------------------------------------------*/
    FBYTE
*find_patch( size ) 
LONG	size ;
{
FBYTE		*ptr ;
BOOLEAN		found ;

    ptr = cpy_bptr ;
    found = FALSE ;
    while ( !found && ptr < cpy_bptr + size )
        {
    	if ( *ptr == LCASE_Z )
	    found = lstr_cmp( ADDR( zyxg ), ptr ) ;
        ++ptr ;
	}
    if ( !found )
        return( LNULLPTR ) ;
    ptr += strlen( zyxg ) - 1 ;
    return( ptr ) ;
} /* find_patch() */

/*----------------------------------------------------------------------*/
    BOOLEAN
do_patch( size, f_name, src1, src, dst )
LONG	*size ;
BYTE	*f_name, *src1, *src, *dst ;
{
FBYTE		*ptr ;
WORD		length ;

    if (!strcmp( f_name, desktopinf ) )
        {
	if ( src == dst )
	    return( dtinf_merge( src1, size ) ) ;
	else
            return( dtinf_patch( size ) ) ;
	}
    ptr = find_patch( *size ) ;
    if ( ptr == LNULLPTR )
        return( FALSE ) ;
    if (!strcmp( f_name, gemvdiexe ) )
        {
	*ptr = (BYTE)( config != HARD_DISK ) ;	/* suppress font paging */
	return( TRUE ) ;
	}
    *ptr++ = (BYTE)( devices[ cur_dev ].ioport ) ;
    if ( devices[ cur_dev ].type == SCRN_TYPE || 
	    devices[ cur_dev ].type == SCAN_TYPE )
        *ptr++ = (BYTE)(devices[ cur_dev ].mouse) ;
    b_stuff( ptr, NAME_LENGTH + DESC_LENGTH + 1, NULL ) ;
    length = _min( NAME_LENGTH, strlen( devices[ cur_dev ].s_desc ) + 1 ) ;
    LBCOPY( ptr, ADDR( devices[ cur_dev ].s_desc ), length ) ;
    ptr += NAME_LENGTH ;
    length = _min( DESC_LENGTH + 1, strlen( devices[ cur_dev ].desc ) + 1 ) ;
    LBCOPY( ptr, ADDR( devices[ cur_dev ].desc ), length ) ;
    return( TRUE ) ;
} /* do_patch() */

/*----------------------------------------------------------------------*/
int create_file( const char *path, const char *name, int mode )
{
    BYTE	file_name[ FNAME_LENGTH ] ;

    strcpy( file_name, path ) ;
    strcat( file_name, name ) ;
    return( dos_create( file_name, mode ) ) ;
} /* create_file() */

/*----------------------------------------------------------------------*/
    BOOLEAN
enough_space( path, name, drive )
BYTE	*path, *name ;
BYTE	drive ;
{
BYTE	file_name[ FNAME_LENGTH ] ;
LONG	file_size, disk_avail, disk_total ;

    strcpy( file_name, path ) ;
    strcat( file_name, name ) ;
    file_size = dos_fsize( file_name ) ;
    dos_space( (WORD)(drive - UCASE_A + 1 ), &disk_total, &disk_avail ) ;
    if ( file_size <= disk_avail ) 
	    return( TRUE );
    return( FALSE );
} /* enough_space() */

/*----------------------------------------------------------------------*/
/* length of buffer = 64K, so that every driver and GEM.EXE will fit.	*/
/* When patch == TRUE, entire file is always in memory.			*/
BOOLEAN file_copy( const char *src, const char *dst, 
		const char *f_name, BOOLEAN patch )
{
WORD	src_hndl, dst_hndl ;
BOOLEAN	ok, err ;
LONG	length, wrote ;
UWORD	time, date ;
const char *src_ptr;
BYTE    temp[ FNAME_LENGTH ] ;

    src_ptr = src ;
    err = FALSE ;
    ok = FALSE ;
    strcpy( temp, dst ) ;
    strcat( temp, f_name ) ;
    /*if (!strcmp( f_name, desktopinf ) && dos_sfirst( ADDR( temp ), 0x0 ) )
        src_ptr = dst ;*/
    do_echo( src_ptr, dst, f_name ) ;
/* if !strcmp( dst, src_ptr ), this is just a change to an already installed */
    if ( !strcmp( src_ptr, dst ) )	/* driver or desktop.inf. */
	{
		del_file(dst, f_name) ;
		err = !enough_space( src_ptr, f_name, dst[ 0 ] ) ;
	}
    if ( !err )
        {
    	src_hndl = open_file( src_ptr, f_name, READ ) ;
    	err = (src_hndl < 0);
	}
    if ( !err )
        {
	length = dos_read( src_hndl, COPY_BSIZE, cpy_bptr ) ; /*read 1st buf*/
/* if !strcmp( dst, src_ptr ), dos_create() will null out original file. */
	dos_getdt( src_hndl, &time, &date ) ;
	dst_hndl = create_file( dst, f_name, READWRITE ) ;
	err = (dst_hndl < 0);
	ok = TRUE ;
	while ( ok & !err )
            {
	    if ( patch )
	    	do_patch( &length, f_name, src, src_ptr, dst ) ;
	    wrote = dos_write( dst_hndl, length, cpy_bptr ) ;
	    length = dos_read( src_hndl, length, cpy_bptr ) ;
	    ok = length > 0L ;
	    }
        dos_close( src_hndl ) ;
    	dos_close( dst_hndl ) ;
	dst_hndl = open_file( dst, f_name, READWRITE ) ;
        dos_setdt( dst_hndl, time, date ) ;
    	dos_close( dst_hndl ) ;
	}
    fin_copy_echo( !err, src_ptr, dst, f_name ) ;
    return( !err ) ;
} /* file_copy() */

/*----------------------------------------------------------------------*/
VOID copy_scan_com(void)
{
BYTE	com_file[NAME_LENGTH];
WORD	ii;

	for (ii = 0 ; ii < num_devs; ii++)
		if (*devices[ii].scan_com != NULL)
			{
			strcpy(com_file, devices[cur_dev].scan_com);
			init_copy_echo();
			file_copy(devices[cur_dev].src_path, gemroot, com_file, TRUE);
			return;
			}
} /* copy_scan_com() */

/*----------------------------------------------------------------------*/
/* length of buffer = 64K, so that every driver will fit.		*/
/* Gets zxyg info from an already installed driver.			*/
    VOID
get_drvr( drvr_name )
BYTE	*drvr_name ;
{
WORD	hndl ;
FBYTE	*ptr ;
BYTE	tmp[ FNAME_LENGTH ] ;
LONG	length ;
DEV_STRUCT	*dev_ptr ;

    dev_ptr = &devices[ num_devs ] ;
    strcpy( dev_ptr->f_name, drvr_name );
    dev_ptr->font_wcard[ 0 ] = ASTERISK ;
    strcpy( &dev_ptr->font_wcard[ 1 ], pos_dot( dev_ptr->f_name ) ) ;
    strcpy( tmp, gemsys ) ;
    strcat( tmp, drvr_name ) ;
    hndl = dos_open( tmp, READ ) ;
    if ( hndl < 0)
        return ;
    length = dos_read( hndl, COPY_BSIZE, cpy_bptr ) ; 
    ptr = find_patch( length ) ;
    if ( ptr != LNULLPTR )
        add_device( ptr ) ;
    dos_close( hndl ) ;
} /* get_drvr() */

/* end of gsfilcpy.c */
