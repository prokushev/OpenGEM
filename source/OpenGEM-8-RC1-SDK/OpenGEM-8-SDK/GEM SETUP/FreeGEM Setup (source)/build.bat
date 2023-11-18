@echo off
set path=e:\pc\bin;%path%
pacc -O -c -Bs gsmain.c
if errorlevel 1 goto stop
pacc -O -c -Bs gsnewcfg.c
if errorlevel 1 goto stop
pacc -O -c -Bs gssetup.c
if errorlevel 1 goto stop
pacc -O -c -Bs gsutil.c
if errorlevel 1 goto stop
pacc -O -c -Bs gsfilcpy.c
if errorlevel 1 goto stop
pacc -O -c -Bs gsdvinst.c
if errorlevel 1 goto stop
pacc -O -c -Bs gsdualfp.c
if errorlevel 1 goto stop
pacc -O -c -Bs pcdos.c
if errorlevel 1 goto stop
pacc -O -c -Bs gsnewdev.c
if errorlevel 1 goto stop
pacc -Bs gsmain.obj gsnewcfg.obj gsnewdev.obj gssetup.obj gsutil.obj gsfilcpy.obj gsdvinst.obj gsdualfp.obj pcdos.obj
if errorlevel 1 goto stop
del gemsetup.exe
ren gsmain.exe gemsetup.exe
:stop
