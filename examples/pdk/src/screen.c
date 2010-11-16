/*
 * screen.c
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "dma.h"
#include "gs.h"
#include "vram.h"
#include "screen.h"
#include "output.h"
#include <kernel.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// temp  temp

u32 GS_FLOAT_ENCODE(float f)
{
	return *((u32*)&f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pdkGsEnv* pdkScreenSetup()
{
  u64 disp;
  static pdkGsEnv env;
  static u8 tempDma[512] ALIGNED(16);
  u64* dmaList = (u64*)&tempDma; // uncached

  env.Height  = 256;
  env.Width   = 640;
  env.Psm     = 0;
  env.Flip    = 0;

  env.Screen0 = pdkVramAllocStatic(env.Width*env.Height*4);
  env.Screen1 = pdkVramAllocStatic(env.Width*env.Height*4);
  env.Zbuffer = pdkVramAllocStatic(env.Width*env.Height*4);

  GsPutIMR(0x0000f700);
  SetGsCrt(1,3,1);

  GS_WRITE( PMODE,(u64)0x0000000000000066 );
  *THE_SMODE2 = 3;

  GS_WRITE(DISPFB_2, GS_DISPFB(env.Screen1>>13,env.Width/64,env.Psm,0,0));

	disp =	(((u64)(((2560+env.Width-1)/env.Width)-1) << 23) |
					 ((u64)((env.Height<<(1?1:0))-1) << 44) |
					 (((u64)656 + (0*((2560+env.Width-1)/env.Width)))&0xfff)|
					 ((u64)((72+0) & 0xfff)<<12) |
					 ((u64)((((2560+env.Width-1)/env.Width)*env.Width)-1) << 32));

  GS_WRITE(DISPLAY_2, disp);

  // Build dmalist
  
  *dmaList++ = DMA_CNT_TAG(10); 
  *dmaList++ = 0;

  *dmaList++ = GS_GIFTAG(9, GS_GIFTAG_EWITHOUT, 0, 0, GS_GIFTAG_PACKED, 1); 
  *dmaList++ = GS_GIFTAG_AD;

  *dmaList++ = GS_PRMODECONT(1); 
  *dmaList++ = GS_REG(PRMODECONT );

  *dmaList++ = GS_FRAME(env.Screen1>>13, env.Width/64, env.Psm, 0); 
  *dmaList++ = GS_REG(FRAME_1);

  *dmaList++ = GS_XYOFFSET(2048 << 4, 2048 << 4); 
  *dmaList++ = GS_REG(XYOFFSET_1);

  *dmaList++ = GS_SCISSOR(0,env.Width-1, 0, env.Height-1); 
  *dmaList++ = GS_REG(SCISSOR_1);

  *dmaList++ = GS_ZBUF(env.Zbuffer/8192, GS_ZBUF_FORMAT(Z32), 0); 
  *dmaList++ = GS_REG(ZBUF_1);

  *dmaList++ = GS_TEST( GS_TEST_AOFF, GS_TEST_AALWAYS, 0xff, GS_TEST_AKEEP, 
                           GS_TEST_DAOFF, 0, GS_TEST_ZOFF, GS_TEST_ZALWAYS); 
  *dmaList++ = GS_REG(TEST_1);

  *dmaList++ = GS_COLCLAMP(GS_COLCLAMP_ON); 
  *dmaList++ = GS_REG(COLCLAMP);

  *dmaList++ = GS_DIMX(-4,2,-3,3,0,-2,1,-1,-3,3,-4,2,1,-1,0,-2); 
  *dmaList++ = GS_REG(DIMX);

  *dmaList++ = GS_DTHE(GS_DTHE_ON); 
  *dmaList++ = GS_REG(DTHE);

  *dmaList++ = DMA_END_TAG(0); 
  *dmaList++ = 0;

  pdkDmaSendChain(tempDma);
  return &env;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkScreenVsync(pdkGsEnv* env)
{
  GS_WRITE(CSR,((GS_READ(CSR)|GS_CSR(0,0,0,GS_CSR_VSON,0,0,0,0,0,0,0,0))));
  while( !((GS_READ(CSR)&GS_CSR(0,0,0,GS_CSR_VSON,0,0,0,0,0,0,0,0))) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkScreenFlip(pdkGsEnv* env)
{
  u32  t;
  static u8 tempDma[64] ALIGNED(16);
  u64* dmaList = (u64*)&tempDma;

  GS_WRITE( DISPFB_2,GS_DISPFB(env->Screen1>>13,env->Width/64,env->Psm,0,0));

  t = env->Screen0; env->Screen0 = env->Screen1; env->Screen1 = t;

  *dmaList++ = DMA_END_TAG(3); 
  *dmaList++ = 0;
  
  *dmaList++ = GS_GIFTAG(2, GS_GIFTAG_EWITHOUT, 0, 0, GS_GIFTAG_PACKED, 1); 
  *dmaList++ = GS_GIFTAG_AD;

  *dmaList++ = GS_FRAME(env->Screen1>>13, env->Width/64, env->Psm, 0); 
  *dmaList++ = GS_REG(FRAME_1);

  *dmaList++ = GS_SCISSOR(0,env->Width-1, 0, env->Height-1 ); 
  *dmaList++ = GS_REG(SCISSOR_1);

  *dmaList++ = GS_XYOFFSET((2048-env->Width/2) << 4,
                           ((2048-env->Height/2) << 4) + env->Flip ? 8 : 0); 
  *dmaList++ = GS_REG(XYOFFSET_1);
  
  env->Flip = !env->Flip;

  pdkDmaSendChain( tempDma );
  pdkDmaWait(2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkScreenClear( pdkGsEnv* env )
{
  static u8 tempDma[128] ALIGNED(16);
  u64* dmaList = (u64*)&tempDma;

  *dmaList++ = DMA_END_TAG(7); 
  *dmaList++ = 0;

  *dmaList++ = GS_GIFTAG(6, GS_GIFTAG_EWITHOUT, 0, 0, GS_GIFTAG_PACKED, 1); 
  *dmaList++ = GS_GIFTAG_AD;

  *dmaList++ = GS_TEST(1, 7, 0xFF, 0, 0, 0, 1, 1 ); 
  *dmaList++ = GS_REG(TEST_1);

  *dmaList++ = GS_PRIM(GS_PRIM_SPRITE, GS_PRIM_SFLAT, GS_PRIM_TOFF, GS_PRIM_FOFF, 
                       GS_PRIM_ABOFF, GS_PRIM_AAOFF, GS_PRIM_FSTQ, GS_PRIM_C1, 0 ); 
  *dmaList++ = GS_REG(PRIM);

  *dmaList++ = GS_RGBAQ(30,20,40,0,0.0f); 
  *dmaList++ = GS_REG(RGBAQ);

  *dmaList++ = GS_XYZ2((2048+0)<<4, (2048+0)<<4, 0); 
  *dmaList++ = GS_REG(XYZ2);

  *dmaList++ = GS_XYZ2((2048+640)<<4, (2048+256)<<4, 0); 
  *dmaList++ = GS_REG(XYZ2);

  *dmaList++ = GS_TEST(1, 7, 0xFF, 0, 0, 0, 1, 2); 
  *dmaList++ = GS_REG(TEST_1);

  pdkDmaSendChain(tempDma);
  pdkDmaWait(2);
}

