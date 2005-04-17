CC = cl.exe
OBJDIR = Debug/
OBJS = $(OBJDIR)Dxf2Kam.obj $(OBJDIR)main.obj
ROOT = d:/!mydata/programming/denisenko/
TARGET = Dxf2Kam
PCH = $(OBJDIR)$(TARGET).pch

COMMCFLAGS = /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /D "_MBCS" /Gm /EHsc /RTC1 /MLd \
	/Fo"$(OBJDIR)" /Fp"$(PCH)" /Fd"$(OBJDIR)$(TARGET).pdb" /W3 /nologo \
	/c /Wp64 /ZI /TP

CFLAGS = $(COMMCFLAGS) /Yu"StdAfx.h"

Dxf2Kam: $(OBJS)
	$(CC) /Fe"$(TARGET).exe" /NOLOGO $(OBJS) $(ROOT)Parsers/Kamea/Lib/Debug/Kamea.lib

$(PCH): StdAfx.cpp StdAfx.h
	$(CC) $(COMMCFLAGS) /Yc "StdAfx.h" StdAfx.cpp

$(OBJDIR)Dxf2Kam.obj: Dxf2Kam.cpp Dxf2Kam.h $(PCH)
	$(CC) $(CFLAGS) Dxf2Kam.cpp

$(OBJDIR)main.obj: main.cpp Dxf2Kam.h $(PCH)
	$(CC) $(CFLAGS) main.cpp
