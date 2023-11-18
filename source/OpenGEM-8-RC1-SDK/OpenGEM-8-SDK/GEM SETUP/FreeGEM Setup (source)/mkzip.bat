@echo off
set TARGET=fgset30.zip

zip %TARGET% 3diskset.txt
zip %TARGET% 6diskset.txt
zip %TARGET% build.bat
zip %TARGET% dosbind.h
zip %TARGET% gemsetup.lbl
zip %TARGET% gemsetup.msg
zip %TARGET% gscreens.h
zip %TARGET% gsdefs.h
zip %TARGET% gsdualfp.c
zip %TARGET% gsdvinst.c
zip %TARGET% gsevars.h
zip %TARGET% gsfilcpy.c
zip %TARGET% gsgvars.h
zip %TARGET% gsmain.c
zip %TARGET% gsnewcfg.c
zip %TARGET% gsnewdev.c
zip %TARGET% gssetup.c
zip %TARGET% gstrings.h
zip %TARGET% gstxtdef.h
zip %TARGET% gsutil.c
zip %TARGET% machine.h
zip %TARGET% pcdos.c
zip %TARGET% portab.h
zip %TARGET% mkzip.bat
zip %TARGET% gemsetup.exe
zip %TARGET% COPYING.txt
zip %TARGET% README.txt
