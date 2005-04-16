CC = cl.exe
OBJDIR = Lib/Debug/
OBJS = $(OBJDIR)Kamea.obj

COMMCFLAGS = /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /D "_MBCS" /Gm /EHsc /RTC1 /MLd \
	/Fo"$(OBJDIR)" /Fp"$(OBJDIR)Kamea.pch" /Fd"$(OBJDIR)Kamea.pdb" /W3 /nologo /c /Wp64 /ZI /TP

CFLAGS = $(COMMCFLAGS) /Yu"stdafx.h"

lib: $(OBJS)
	lib.exe /OUT:"$(OBJDIR)Kamea.lib" /NOLOGO $(OBJS)

$(OBJDIR)Kamea.pch: StdAfx.cpp StdAfx.h
	$(CC) $(COMMCFLAGS) /Yc "StdAfx.h" StdAfx.cpp

$(OBJDIR)Kamea.obj: Kamea.cpp Kamea.h $(OBJDIR)Kamea.pch
	$(CC) $(CFLAGS) Kamea.cpp
