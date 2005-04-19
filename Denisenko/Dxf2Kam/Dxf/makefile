CC = cl.exe
OBJDIR = Lib/Debug/
OBJS = $(OBJDIR)Dxf.obj
ROOT = d:/!mydata/programming/denisenko/
TARGET = Dxf
PCH = $(OBJDIR)$(TARGET).pch

COMMCFLAGS = /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /D "_MBCS" /Gm /EHsc /RTC1 /MLd \
	/Fo"$(OBJDIR)" /Fp"$(PCH)" /Fd"$(OBJDIR)$(TARGET).pdb" /W3 /nologo \
	/c /Wp64 /ZI /TP

CFLAGS = $(COMMCFLAGS) /Yu"StdAfx.h"

Dxf: $(OBJS)
	lib.exe /OUT:"$(OBJDIR)$(TARGET).lib" /NOLOGO $(OBJS)

$(PCH): StdAfx.cpp StdAfx.h
	$(CC) $(COMMCFLAGS) /Yc "StdAfx.h" StdAfx.cpp

$(OBJDIR)Dxf.obj: Dxf.cpp Dxf.h $(PCH)
	$(CC) $(CFLAGS) Dxf.cpp
