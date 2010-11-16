/*
 * gs.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "dma.h"
#include "dmaregs.h"
#include "output.h"
#include <tamtypes.h>
#include <kernel.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkDmaSend(void* data,u32 quadSize)
{
  pdkDmaWait(2);
  
	asm __volatile__("sync.l");
  FlushCache(0);
  
  *D2_MADR = (u32)data;
  *D2_QWC  = quadSize;
  *D2_CHCR = 0x100;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkDmaSendChain(void* data)
{
  pdkDmaWait(2);
  
	asm __volatile__("sync.l");
  FlushCache(0);
  
  *D2_TADR = (u32)data;
  *D2_QWC  = 0;
  *D2_CHCR = DMA_CHCR(0,1,0,0,0,1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkDma01SendChain(void* data)
{
  pdkDmaWait( 2 );
  
	asm __volatile__("sync.l");
  FlushCache(0);
  
  *D1_MADR = 0;
  *D1_TADR = (u32)data;
  *D1_QWC  = 0;
  *D1_CHCR = DMA_CHCR(1,1,0,1,0,1);
}

void pdkDmaStat();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkDmaWait(int channel)
{
  int count = 0;

  switch(channel)
  {
    case 2 :
    {
      while( *D2_CHCR&0x100 )
      {
        asm __volatile__( "nop;nop;nop;nop;nop;nop;nop;nop;" );
      }
      break;
    }

    case 1 :
    {
      while( *D1_CHCR&0x100 )
      {
        count++;
        asm __volatile__( "nop;nop;nop;nop;nop;nop;nop;nop;" );
      
        if(count == 50000)
        {
          pdkOut("banz0r!\n");
          return;
        }
      }
      break;
    }
  }
}

