CC = cl.exe
DEBUG = /Od /D "_DEBUG"  /ZI /Fd"$(OBJDIR)$(TARGET).pdb"
OBJDIR = Debug/
OBJS = $(OBJDIR)Dxf2Kam.obj $(OBJDIR)main.obj
ROOT = d:/!mydata/programming/denisenko/
TARGET = Dxf2Kam
PCH = $(OBJDIR)$(TARGET).pch

COMMCFLAGS = $(DEBUG) /EHsc /RTC1 /MLd /Fo"$(OBJDIR)" /Fp"$(PCH)" /c

CFLAGS = $(COMMCFLAGS) /Yu"StdAfx.h"
LFLAGS = /Fe"$(OBJDIR)$(TARGET).exe" $(DEBUG) 

Dxf2Kam: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) Kamea/Lib/Debug/Kamea.lib Dxf/Lib/Debug/Dxf.lib

$(PCH): StdAfx.cpp StdAfx.h
	$(CC) $(COMMCFLAGS) /Yc "StdAfx.h" StdAfx.cpp

$(OBJDIR)Dxf2Kam.obj: Dxf2Kam.cpp Dxf2Kam.h $(PCH)
	$(CC) $(CFLAGS) Dxf2Kam.cpp

$(OBJDIR)main.obj: main.cpp Dxf2Kam.h $(PCH)
	$(CC) $(CFLAGS) main.cpp
