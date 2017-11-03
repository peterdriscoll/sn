# Microsoft Developer Studio Project File - Name="sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sn - Win32 DS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sn.mak" CFG="sn - Win32 DS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sn - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sn - Win32 DS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sn - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "c:\nova\lib"
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SN_BUILD" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib aco_dll.lib pgc.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "sn - Win32 DS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sn___Win32_DS"
# PROP BASE Intermediate_Dir "sn___Win32_DS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\nova\lib"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SN_BUILD" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SN_BUILD" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 aco_dll.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"c:\nova\lib\sn.dll" /pdbtype:sept
# ADD LINK32 aco_dll.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib pgc.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "sn - Win32 Release"
# Name "sn - Win32 DS"
# Begin Group "Main Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sni.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_add.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_and.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_bool.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_buildset.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_char.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_commutative.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_comparison.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_concat.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_define.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_divide.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_double.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_equals.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_error.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_expression.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_function.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_functiondef.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_greaterequals.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_greaterthan.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_if.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_in.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_lambda.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_lessequals.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_lessthan.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_local.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_long.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_meta.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_multiply.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_namable.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_not.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_null.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_number.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_object.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_or.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_replacement.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_rev_commutative.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_selectleft.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_selectright.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_set.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_square.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_squareroot.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_string.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_stringref.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_subtract.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_subtractleft.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_subtractright.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_taggedvalue.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_value.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_valueset.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_vardef.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_variable.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_world.cpp
# End Source File
# Begin Source File

SOURCE=.\sni_worldset.cpp
# End Source File
# End Group
# Begin Group "Main Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\sni.h
# End Source File
# Begin Source File

SOURCE=.\sni_add.h
# End Source File
# Begin Source File

SOURCE=.\sni_and.h
# End Source File
# Begin Source File

SOURCE=.\sni_bool.h
# End Source File
# Begin Source File

SOURCE=.\sni_buildset.h
# End Source File
# Begin Source File

SOURCE=.\sni_char.h
# End Source File
# Begin Source File

SOURCE=.\sni_commutative.h
# End Source File
# Begin Source File

SOURCE=.\sni_comparison.h
# End Source File
# Begin Source File

SOURCE=.\sni_concat.h
# End Source File
# Begin Source File

SOURCE=.\sni_define.h
# End Source File
# Begin Source File

SOURCE=.\sni_divide.h
# End Source File
# Begin Source File

SOURCE=.\sni_double.h
# End Source File
# Begin Source File

SOURCE=.\sni_equals.h
# End Source File
# Begin Source File

SOURCE=.\sni_error.h
# End Source File
# Begin Source File

SOURCE=.\sni_expression.h
# End Source File
# Begin Source File

SOURCE=.\sni_function.h
# End Source File
# Begin Source File

SOURCE=.\sni_functiondef.h
# End Source File
# Begin Source File

SOURCE=.\sni_greaterequals.h
# End Source File
# Begin Source File

SOURCE=.\sni_greaterthan.h
# End Source File
# Begin Source File

SOURCE=.\sni_if.h
# End Source File
# Begin Source File

SOURCE=.\sni_in.h
# End Source File
# Begin Source File

SOURCE=.\sni_lambda.h
# End Source File
# Begin Source File

SOURCE=.\sni_lessequals.h
# End Source File
# Begin Source File

SOURCE=.\sni_lessthan.h
# End Source File
# Begin Source File

SOURCE=.\sni_local.h
# End Source File
# Begin Source File

SOURCE=.\sni_long.h
# End Source File
# Begin Source File

SOURCE=.\sni_meta.h
# End Source File
# Begin Source File

SOURCE=.\sni_multiply.h
# End Source File
# Begin Source File

SOURCE=.\sni_namable.h
# End Source File
# Begin Source File

SOURCE=.\sni_not.h
# End Source File
# Begin Source File

SOURCE=.\sni_null.h
# End Source File
# Begin Source File

SOURCE=.\sni_number.h
# End Source File
# Begin Source File

SOURCE=.\sni_object.h
# End Source File
# Begin Source File

SOURCE=.\sni_or.h
# End Source File
# Begin Source File

SOURCE=.\sni_replacement.h
# End Source File
# Begin Source File

SOURCE=.\sni_rev_commutative.h
# End Source File
# Begin Source File

SOURCE=.\sni_selectleft.h
# End Source File
# Begin Source File

SOURCE=.\sni_selectright.h
# End Source File
# Begin Source File

SOURCE=.\sni_set.h
# End Source File
# Begin Source File

SOURCE=.\sni_square.h
# End Source File
# Begin Source File

SOURCE=.\sni_squareroot.h
# End Source File
# Begin Source File

SOURCE=.\sni_string.h
# End Source File
# Begin Source File

SOURCE=.\sni_stringref.h
# End Source File
# Begin Source File

SOURCE=.\sni_subtract.h
# End Source File
# Begin Source File

SOURCE=.\sni_subtractleft.h
# End Source File
# Begin Source File

SOURCE=.\sni_subtractright.h
# End Source File
# Begin Source File

SOURCE=.\sni_taggedvalue.h
# End Source File
# Begin Source File

SOURCE=.\sni_value.h
# End Source File
# Begin Source File

SOURCE=.\sni_valueset.h
# End Source File
# Begin Source File

SOURCE=.\sni_vardef.h
# End Source File
# Begin Source File

SOURCE=.\sni_variable.h
# End Source File
# Begin Source File

SOURCE=.\sni_world.h
# End Source File
# Begin Source File

SOURCE=.\sni_worldset.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Interface Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\inc\sn.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_bool.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_char.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_define.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_double.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_error.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_expression.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_function.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_functiondef.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_lambda.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_local.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_long.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_meta.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_null.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_number.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_object.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_parameter.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_set.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_string.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_stringref.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_value.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_valueset.h
# End Source File
# Begin Source File

SOURCE=..\inc\sn_variable.h
# End Source File
# End Group
# Begin Group "Interface Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\sn.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_bool.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_char.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_define.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_double.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_error.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_expression.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_function.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_functiondef.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_lambda.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_local.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_long.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_meta.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_null.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_number.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_object.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_parameter.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_set.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_string.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_stringref.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_value.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_valueset.cpp
# End Source File
# Begin Source File

SOURCE=.\sn_variable.cpp
# End Source File
# End Group
# Begin Group "Utility Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\logcontext.h
# End Source File
# Begin Source File

SOURCE=.\utility.h
# End Source File
# End Group
# Begin Group "Utility Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\logcontext.cpp
# End Source File
# Begin Source File

SOURCE=.\utility.cpp
# End Source File
# End Group
# End Target
# End Project
