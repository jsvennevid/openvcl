/*
 * vram.c
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "vram.h"

#define BLOCKSIZE  8192
#define BLOCKSIZEW 64
#define BLOCKSIZEH 32

static u32 vramStaticEnd = (-BLOCKSIZE)&(0+BLOCKSIZE-1);
static u32 vramDynOffset = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u32 pdkVramAllocStatic(u32 size)
{
	size = (-BLOCKSIZE)&(size+BLOCKSIZE-1);
  vramStaticEnd += size;
  vramDynOffset = vramStaticEnd;
  return vramStaticEnd-size;
}

