# Makefile.am - MASP Assembly Preprocessor Makefile
# Copyright 2003 Johann Gunnar Oskarsson

# Written by Johann Gunnar Oskarsson
#    myrkraverk@users.sourceforge.net

# This file is part of MASP, the Assembly Preprocessor.

# MASP is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# MASP is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with MASP; see the file COPYING.  If not, write to the Free
# Software Foundation, 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

## Process this file with automake to generate Makefile.in

## Work around apparent automake bug.
#INTLLIBS = @INTLLIBS@

bin_PROGRAMS = masp
masp_SOURCES = masp.c macro.c sb.c hash.c
masp_LDADD = ../libiberty/libiberty.a

EXTRA_DIST = as.h asintl.h bignum.h bin-bugs.h expr.h flonum.h fopen-same.h \
	hash.h macro.h progress.h sb.h 

#masp.o: masp.c getopt.h safe-ctype.h \
#    sb.h macro.h xregex.h xregex2.h


