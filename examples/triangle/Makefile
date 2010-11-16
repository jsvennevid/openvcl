# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id: Makefile.sample,v 1.2 2004/09/14 14:41:25 pixel Exp $

EE_INCS = -I../pdk/src
EE_LDFLAGS = -L../pdk/lib
EE_BIN = triangle.elf
EE_OBJS = main.o vu1Triangle.o
EE_LIBS = -lc -lkernel -lsyscall -lpdk -lc
EE_DVP = dvp-as
VCL = ../../openvcl

all: $(EE_BIN) $(VU_OBJECTS) 

vu1Triangle.o : vu1Triangle.vsm
	$(EE_DVP) $< -o $@

clean:
	rm -f *.elf *.o *.a *.vsm

%.vsm : %.vcl
	$(VCL) -o $@ $<


		
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal


