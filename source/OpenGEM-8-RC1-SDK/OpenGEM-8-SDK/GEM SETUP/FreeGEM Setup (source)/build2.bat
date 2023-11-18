@echo off
call c:\watcom\owsetenv
set INCLUDE=%WATCOM%\H;%WATCOM%\H\os21x
rem wcl -0 -bt=dos -ml gsmain.c gsnewcfg.c gssetup.c gsutil.c gsfilcpy.c gsdvinst.c gsdualfp.c pcdos.c gsnewdev.c
wcl -0 -bt=dos -ml gsmain.c gsnewcfg.c gssetup.c gsutil.c gsfilcpy.c gsdvinst.c gsdualfp.c pcos2.c gsnewdev.c
if exist gemsetup.exe del gemsetup.exe
ren gsmain.exe gemsetup.exe
