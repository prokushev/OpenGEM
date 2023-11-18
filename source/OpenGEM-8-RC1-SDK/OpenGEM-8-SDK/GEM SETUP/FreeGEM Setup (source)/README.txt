FreeGEM Setup 3.2                                  Yuri Prokushev, 2022-??-?? 
==============================================================================

  This is an attempt to update the updated source of FreeGEM Setup (3.0-pacc1) 
so that it compiles with the Watcom C compiler.

  Only one build configuration is available - FREEGEM_SETUP, selected
using a #define at the top of gsdefs.h. 

#define FREEGEM_SETUP 0   to build a drop-in replacement for the Digital 
                         Research GEMSETUP, suitable for installing GEM/3.

#define FREEGEM_SETUP 1   to build a modified version that installs the extra
                         system files associated with FreeGEM.

FREEGEM_SETUP 0 is not tested and not supported. It is subject to remove.

What's New
~~~~~~~~~~
  The following changes have been made, compared to the Digital Research 
original:

- Supports more than 16 disk drives, in case you want to install GEM on 
 a drive beyond P:. The code which scans drives has also been redesigned,
 and will hopefully handle systems without any floppy drives.

- Should support installing GEM from a hard drive directory rather than 
 insisting on drive A:. This requires a GEMSETUP.LBL file in the directory,
 containing disk labels for those disks which the directory contains, with
 each label enclosed by {braces}. For the standard GEM/3 "three-disk" 
 installation, for example, it would contain at least three lines:

	{GEM  SYSTEM}
	{GEM DESKTOP}
	{GEM  SCREEN}

  A sample gemsetup.lbl file has been included.

- Supports custom screen colours. These are specified in GEMSETUP.MSG, in the
  @COLOURS_PTR section -- see that file for more details.

- If FREEGEM_SETUP is defined, adds a trashcan to the DESKTOP.INF file.

Possible future enhancements
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- Allow the list of GEM system files to be specified in GEMSETUP.TXT, rather 
 than using a hardcoded list. This will allow distributions containing more
 files to be installed (eg, including Paint and Draw). However, it will break
 backward compatibility with GEM/3 GEMSETUP.TXT files.

- Allow the path for driver packs to be entered, rather than hardcoding A:\.

- Allow the target path to be entered, rather than hardcoding 
 \GEMAPPS\GEMSYS. This will require functionality from RESDIR, to regenerate
 GEM.RSC during the installation.

Not tested
~~~~~~~~~~

  Large areas of the code haven't been tested. These include the code to 
install GEM to a floppy disk, or to two separate floppies ('startup' and 
'desktop'). It is strongly advised to take backups before testing this 
code -- even in the tested bits, there may be lurking traps or semantic 
differences between Pacific C and High C.

Change history
~~~~~~~~~~~~~~

* 2022-??-??: FreeGEM Setup 3.2 released.

	- Ported to compile with Watcom C rather than Pacific C.
        - Ported to Family API from BIOS/DOS API (now it is dual mode DOS and OS/2 application)
	- Multilanguage setup (example in Russian).
	- File-list moved to GEMSETUP.TXT file.
	- Added translation documentation.
	- Documentation updated to confirm OpenGEM distro.

* 2013-05-15: FreeGEM Setup 3.0-pacc1 released.

	- Rewritten to compile with Pacific C rather than Metaware High C.
	- Supports installation from a hard drive directory rather than 
	 multiple floppies.
	- Screen colours can be customised using settings in GEMSETUP.MSG.
	- Floppy / hard drive probe rewritten. Now supports up to 32 drives.
	- Logo uses characters 0xDF / 0xE0 rather than 0x1E/0x1F, so they
	 join up correctly.
	- Does not attempt to draw the logo on CGA screens, since these do not 
	 support character redefinition. Also omits the logo on Windows NT,
	 since the NT DOS window doesn't support character redefinition 
	 either.
	- Compile-time option to install DRI GEM/3 or FreeGEM.

* 1988-11-03: Digital Research GEMSETUP 3.11 released.

	- Last known binary release of GEM/3 Setup from Digital Research.

* 1988-01-28: Digital Research GEMSETUP 3.0B3 released.

	- Last known source release from Digital Research.

License
~~~~~~~

Copyright 2020, Yuri Prokushev <yuri.prokushev@gmail.com>
Copyright 2013, John Elliott <jce@seasip.demon.co.uk>
Copyright 1999, Caldera Thin Clients, Inc. 

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original documentation
~~~~~~~~~~~~~~~~~~~~~~

  The original Digital Research documentation is appended, unchanged. The 
instructions for building can be ignored, but the description of the 
GEMSETUP.TXT file format remains current.

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

               - GUIDELINES FOR BUILDING GEMSETUP 3.0 -

                              Cregg Lund
                         Digital Research Inc.
                            (408) 646-6397

  The source files for gemsetup are arranged in 2 sub-directories.  The
first sub_directory is called SETUP.  The files in SETUP are all the 
C and ASSEMBLY code for GEMSETUP.  The second sub_directory is called
BINDINGS.  The files in BINDINGS are all the source files for a DOS 
SYSTEM CALL library.  The directory structure is as follows:

			        C:\
			        |
			        |
                 ---------------------------------
                |                                |
                |                                |
             \SETUP                          \BINDINGS




  The following tools were used for GEMSETUP:
  
  	C COMPILER: 	METAWARE HIGH C version 1.4
         ASSEMBLER:     MICROSOFT MACRO ASSEMBLER version 3.00
	    LINKER:     PLINK86 (Phoenix Software) version 1.47
        LIB-LINKER:     PLIB86 (Phoenix Software) version 1.4
    

  To compile and assemble the source code files in SETUP, run the
batch file called COMPILE.BAT.

  To link the object files in SETUP, run the batch file called LINK.BAT.

  To build the library in BINDINGS, run the batch file called DOSLIB.BAT.

  GEMSETUP reads an input file which contains information about the various
devices supported by GEM.  The file is called GEMSETUP.TXT.  There are two
files in SETUP that end with TXT.  One is called 6DISKSET.TXT and is used for
5 1/4" disks; the other is called 3DISKSET.TXT and is used for 3 1/2" disks.
For whichever media is used (3 1/2" or 5 1/4"), the appropriate TXT file
must renamed to GEMSETUP.TXT.   

    The following is a list of labels used in the GEMSETUP.TXT file and 
their descriptions:


	|DESCRIPTION|	72 character-long description of the driver/device
		This label is used by GEMSETUP.EXE in its device choice menus.

	|SHORT DESCRP|	12 character-long icon label for the device. Used
		by OUTPUT.APP.

	|LONG DESCRP|	paragraph of information (in 72 character-long lines).
		When the user asks for help in one of GEMSETUP.EXE's device
		choice menus, he/she sees this information. Limited to a
		maximum of 20 lines.

	|FILENAME|	12 characters of filename descriptor for actual driver
		file. Used by GEMSETUP to find file on disk.

	|FNT WILDCRD|	Wildcard of the form "*.EGA;*.EGC". GEMSETUP uses
		these wildcards to search for fonts for each driver. Usually
		this wildcard simply matches the extension of FILENAME, above.

	|MOUSE_ID|	Mouse id value for screen drivers. Currently in the
		range 0-10. GEMSETUP patches this value in the zyxg area
		in the screen drivers. 

	|SRC DISK|	Diskette label for disk containing FILENAME. Used in
		GEMSETUP's prompt to the user asking for a certain disk to
		be inserted.

	|FONT DISK|	Diskette label for disk containing associated fonts
		for a driver. GEMSETUP asks the user to insert this disk,
		and then copies the files specified in FNT WILDCRD.

	|SCAN PORT|	Port address value for scanner. This value is patched
		by GEMSETUP in the zyxg area of the scanner driver.

	|SCAN XFER|	Transfer address value for scanner. This value is
		patched by GEMSETUP in the zyxg area of the scanner driver.

	|SCAN COM|	File specification of any manufacturer-supplied
		dos driver-type utilities required by the scanner. GEMSETUP
		asks for the disk and copies the file into \GEMAPPS\GEMSYS.

	|AUX FILES|	File specification of any additional DRI-supplied
		files required by the driver. GEMSETUP will not ask for the 
		disk, but assume that the file is on the same disk as the 
		driver. The file will then be copied into \GEMAPPS\GEMSYS.
		(This facility accomodates printers requiring .CNF files.)

The absolute minimum requirement is:

	|DESCRIPTION|
	|FILENAME|

In addition, any device using OUTPUT.APP (not mice, metafiles or scanners) 
requires the following:

	|SHORT DESCRP|

In addtion, all mice and tablets require the following:

	|MOUSE_ID|

In addtion, all scanners require the following:

	|SCAN PORT|
	|SCAN XFER|
	|SCAN COM|

In addtion, all output devices having fonts 1) with a different extension
	than FILENAME or 2) on a different disk than SRC DISK require:

	|FNT WILDCRD|
	|FONT DISK|



= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

	                  G E M S E T U P    3 . 0    

               S T R I N G    S E T   T R A N S L A T I O N  

                            G U I D E L I N E S


                               Cregg Lund			     
			  Digital Research Inc.
			     December, 1987


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

	GEMSETUP gets most of it's character strings from 2 files called:
GEMSETUP.TXT and GEMSETUP.MSG.  The TXT file contains all the choices 
for device drivers.  The MSG file contains prompts, messages, and other
strings used by GEMSETUP.  

	To translate the strings in the MSG file, follow these guidelines:

	1) The beginning and end of each string set is marked by open and 
	   close braces ('{' and '}').  The first character of each string
	   set must immediatly follow the open brace.  The closing brace must
	   be on the next line after the last character of the string set.

	   example:
	   {EXIT TO DOS
	   CONTINUE                                                  
	   }

	2) Above each string set is a message which states the number of 
	   lines that particular string set occupies.  That number must be
	   maintained.  DO NOT add any additional lines.  If after a string
	   is translated it occupies less lines, then leave the last lines 
	   blank.

	3) DO NOT use editors which insert hidden characters (i.e. ESC 
	   characters.)  Also, avoid using tabs.  Use spaces instead.

	4) When translating a line which has a PIPE character ('|') in it,
	   be sure and leave the PIPE in it's original line.  The PIPE can
	   be moved around to any column in the original line.

	5) DO NOT do anything to the lines which begin with AT_SIGNs, ('@').
	   Do not translate them, do not move them, do not touch them.

	6) Don't bother trying to do anything with the lines which say:
	   RESERVED.

	7) The string sets under the @PROMPT_PTR header are indented for 
	   cosmetic reasons.  If more room is needed for translating a 
	   particular line, then use the indentation space; however, keep
	   in mind that unindented screens will look inconsistent with the
	   rest.

	8) Avoid indenting any string sets other than the strings under
	   @PROMPT_PTR.

	   




	To translate the strings in the TXT file, follow these guidelines:

	1) The TXT file is catagorized by hardware types.  Each catagory
	   has a number of devices within it.  Each device has different
	   information associated with it.  The  begininng and ending of the
	   device information is marked with open and close braces ('{' and
	   '}' ).  An example of device information, SCREEN in this case, 
	   follows:

	    {
	    |DESCRIPTION|AT&T DEB Card 16-Color Display (640x400) 
	    |SHORT DESCR|AT&T 16 Clr
	    |FILENAME|SDDEB8.EGA
	    |SRC DISK|GEM SCREEN DISK #1
	    |FNT WILDCRD|*.EGA
	    |LONG DESCRP|
	    Choose this entry if the AT&T DEB Graphics adapter card and 16-
	    color display are installed in your computer.  This display 
	    offers a resolution of 640 horizontal by 400 vertical pixels.
	    }


	2) The strings between the PIPEs ('|') should not be translated.
	   The only strings that should be translated are the strings 
	   following the DESCRIPTION, the SRC DISK, the FONT DISK and the 
	   LONG DESCRP labels.  All other strings are transparent to the 
	   user.  If the 'labels' between the PIPEs are translated, then 
	   the strings under the @KEYWORDS_PTR header in the GEMSETUP.MSG
	   file should be modified to reflect those translations.

	3) The DESCRIPTION strings must not go beyond the end of the line,
	   (i.e. maximum of 64 characters not counting <cr><lf>.)  The
	   DESCRIPTION strings are what the user sees as choices for 
	   screens, printers, etc.

	4) The SCR DISK, and FONT DISK strings must follow the normal DOS
	   file naming conventions (8 characters plus 3 for extension.)
	   These strings must match EXACTLY the strings under the @HLAB_PTR
	   header in the GEMSETUP.MSG file. These strings are what the user
	   sees when asked to "Insert the disk labeled ...".

	5) The LONG DESCRP strings can be up to 10 lines long and the current
	   number of lines used for each LONG DESCRP does not need to be 
	   maintained.  Start the LONG DESCRP string on the next line after
	   the LONG DESCRP label.  These string sets are what the user sees
	   when he pushes <F1> to get device information.
