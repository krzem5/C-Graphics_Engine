@echo off
cls
set _INCLUDE=%INCLUDE%
set INCLUDE=./;../src/include;%INCLUDE%
if exist build rmdir /s /q build
mkdir build
cd build
if %1.==. goto dbg
if %1==-r (
	fxc ..\rsrc\phong.hlsl /T vs_4_0 /E vertex_shader /Zpr /O3 /WX /Zss /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv /Fh phong_vertex.h&&fxc ..\rsrc\phong.hlsl /T ps_4_0 /E pixel_shader /Zpr /O3 /WX /Zss /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv /Fh phong_pixel.h&&fxc ..\rsrc\phong.hlsl /T vs_4_0 /E vertex_shader_tex /Zpr /O3 /WX /Zss /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv /Fh phong_vertex_tex.h&&fxc ..\rsrc\phong.hlsl /T ps_4_0 /E pixel_shader_tex /Zpr /O3 /WX /Zss /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv /Fh phong_pixel_tex.h&&cl /c /permissive- /GS /W3 /Zc:wchar_t /Gm- /sdl /Zc:inline /fp:precise /D "NDEBUG"  /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /errorReport:none /WX /Zc:forScope /Gd /Oi /FC /EHsc /nologo /diagnostics:column /GL /Gy /Zi /O2 /Oi /MD ../src/main.c ../src/g_engine/*.c&&link *.obj /OUT:g_engine.exe /DYNAMICBASE "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /MACHINE:X64 /SUBSYSTEM:CONSOLE /ERRORREPORT:none /NOLOGO /TLBID:1 /WX /LTCG /OPT:REF /INCREMENTAL:NO /OPT:ICF&&goto run
	goto end
)
:dbg
fxc ..\rsrc\phong.hlsl /T vs_4_0 /E vertex_shader /Zpr /O0 /WX /Zss /nologo /Fh phong_vertex.h&&fxc ..\rsrc\phong.hlsl /T ps_4_0 /E pixel_shader /Zpr /O0 /WX /Zss /nologo /Fh phong_pixel.h&&fxc ..\rsrc\phong.hlsl /T vs_4_0 /E vertex_shader_tex /Zpr /O0 /WX /Zss /nologo /Fh phong_vertex_tex.h&&fxc ..\rsrc\phong.hlsl /T ps_4_0 /E pixel_shader_tex /Zpr /O0 /WX /Zss /nologo /Fh phong_pixel_tex.h&&cl /c /permissive- /GS /W3 /Zc:wchar_t /Gm- /sdl /Zc:inline /fp:precise /D "_DEBUG"  /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /errorReport:none /WX /Zc:forScope /Gd /Oi /FC /EHsc /nologo /diagnostics:column /ZI /Od /RTC1 /MDd ../src/main.c ../src/g_engine/*.c&&link *.obj /OUT:g_engine.exe /DYNAMICBASE "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /MACHINE:X64 /SUBSYSTEM:CONSOLE /ERRORREPORT:none /NOLOGO /TLBID:1 /WX /DEBUG /INCREMENTAL&&goto run
goto end
:run
del *.obj
del *.pdb
del *.exp
del *.ilk
del *.idb
del *.h
xcopy ..\rsrc .\rsrc /SEYI
cls
g_engine.exe
:end
cd ../
set INCLUDE=%_INCLUDE%
